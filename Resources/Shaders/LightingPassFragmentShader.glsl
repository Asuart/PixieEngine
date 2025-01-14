#version 330 core

out vec4 FragColor;
  
in vec2 fTexCoord;

const float PI = 3.14159265359;
const float gamma = 2.2;
const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;
const float LUT_BIAS  = 0.5 / LUT_SIZE;
const int MAX_AREA_LIGHTS = 32;
const int MAX_POINT_LIGHTS = 32;

struct AreaLight {
	vec3 emission;
    vec3 points[3];
	bool twoSided;
};
uniform AreaLight areaLights[MAX_AREA_LIGHTS];
uniform int nAreaLights;

struct PointLight {
    vec3 position;
    vec3 emission;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int nPointLights;

uniform vec3 cameraPos;

uniform sampler2D gAlbedoSpec;
uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMetallic;
uniform sampler2D LTC1; // for inverse M
uniform sampler2D LTC2; // GGX norm, fresnel, 0(unused), sphere
uniform sampler2D ssaoTexture;

// Vector form without project to the plane (dot with the normal)
// Use for proxy sphere clipping
vec3 IntegrateEdgeVec(vec3 v1, vec3 v2) {
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985f + (0.4965155f + 0.0145206f * y) * y;
    float b = 3.4175940f + (4.1616724f + y) * y;
    float v = a / b;

    float theta_sintheta = (x > 0.0f) ? v : 0.5f  *inversesqrt(max(1.0f - x * x, 1e-7)) - v;

    return cross(v1, v2) * theta_sintheta;
}

// P is fragPos in world space (LTC distribution)
vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[3], bool twoSided) {
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));
	//Minv = Minv * transpose(mat3(N, T2, T1));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[2] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0f);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);

	// integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z / len;
    if (behind) {
        z = -z;
    }

    vec2 uv = vec2(z * 0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len * scale;
    if (!behind && !twoSided) {
        sum = 0.0f;
    }

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

// PBR-maps for roughness (and metallic) are usually stored in non-linear
// color space (sRGB), so we use these functions to convert into linear RGB.
vec3 PowVec3(vec3 v, float p) {
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec3 ToLinear(vec3 v) { 
    return PowVec3(v, gamma);
}

vec3 ToSRGB(vec3 v) {
    return PowVec3(v, 1.0f / gamma); 
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

void main() {        
    vec3 mAlbedo = texture(gAlbedoSpec, fTexCoord).rgb;
    float mSpecular = texture(gAlbedoSpec, fTexCoord).a;     
    vec3 mWorldPos = texture(gPositionRoughness, fTexCoord).rgb;
    float mRoughness = texture(gPositionRoughness, fTexCoord).a;
    vec3 mNormal = texture(gNormalMetallic, fTexCoord).rgb;
    float mMetallic = texture(gNormalMetallic, fTexCoord).a;
    float mSSAO = texture(ssaoTexture, fTexCoord).x;
    vec3 toCamera = normalize(cameraPos - mWorldPos);
    float dotNV = clamp(dot(mNormal, toCamera), 0.0f, 1.0f);

    vec2 lut_uv = vec2(mRoughness, sqrt(1.0f - dotNV));
    lut_uv = lut_uv * LUT_SCALE + LUT_BIAS;
    // get 4 parameters for inverse_M
    vec4 t1 = texture(LTC1, lut_uv);
    // Get 2 parameters for Fresnel calculation
    vec4 t2 = texture(LTC2, lut_uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    vec3 ambientLighting = mAlbedo * 0.3f * mSSAO;

    vec3 areaLighting = vec3(0.0f);
	for (int i = 0; i < nAreaLights; i++) {
		vec3 diffuse = LTC_Evaluate(mNormal, toCamera, mWorldPos, mat3(1), areaLights[i].points, areaLights[i].twoSided);
		vec3 specular = LTC_Evaluate(mNormal, toCamera, mWorldPos, Minv, areaLights[i].points, areaLights[i].twoSided);

		// GGX BRDF shadowing and Fresnel
		// t2.x: shadowedF90 (F90 normally it should be 1.0)
		// t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
		specular *= mSpecular * t2.x + (1.0f - mSpecular) * t2.y;

		areaLighting += areaLights[i].emission * (specular + mAlbedo * diffuse);
	}

    vec3 pointLighting = vec3(0.0f);
	for(int i = 0; i < nPointLights; i++) {
		vec3 toLight = normalize(pointLights[i].position - mWorldPos);
		vec3 halfVector = normalize(toCamera + toLight);
        float dotNL = max(dot(mNormal, toLight), 0.0f);

		float distance = length(pointLights[i].position - mWorldPos);
		float attenuation = 1.0f / (distance * distance);
		vec3 radiance = pointLights[i].emission * attenuation;

		vec3 F0 = vec3(0.04f);
		F0 = mix(F0, mAlbedo, mMetallic);
		vec3 F = FresnelSchlick(max(dot(halfVector, toCamera), 0.0f), F0);

		float NDF = DistributionGGX(mNormal, halfVector, mRoughness);
		float G = GeometrySmith(mNormal, toCamera, toLight, mRoughness);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0f * dotNV * dotNL + 0.0001f;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0f) - kS;
		kD *= 1.0f - mMetallic;

		pointLighting += (kD * mAlbedo / PI + specular) * radiance * dotNL;
	}
    
    vec3 lighting = ambientLighting + areaLighting + pointLighting;

    vec3 color = lighting / (lighting + vec3(1.0f));
	color = pow(color, vec3(1.0f / 2.2f));

    FragColor = vec4(color, 1.0f);
} 
