#version 330 core

out vec4 FragColor;

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fNormal;

const float PI = 3.14159265359;
const float gamma = 2.2;
const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0) / LUT_SIZE;
const float LUT_BIAS  = 0.5 / LUT_SIZE;
const int MAX_POINT_LIGHTS = 32;
const int MAX_AREA_LIGHTS = 32;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D LTC1; // for inverse M
uniform sampler2D LTC2; // GGX norm, fresnel, 0(unused), sphere
uniform sampler2D brdfLUTMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

uniform vec3 uAlbedo;
uniform float uMetallic;
uniform float uRoughness;

uniform vec3 uCameraPos;

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

vec3 GetNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, fTexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fWorldPos);
    vec3 Q2  = dFdy(fWorldPos);
    vec2 st1 = dFdx(fTexCoords);
    vec2 st2 = dFdy(fTexCoords);

    vec3 N  = normalize(fNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 IntegrateEdgeVec(vec3 v1, vec3 v2) {
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206 * y) * y;
    float b = 3.4175940 + (4.1616724 + y) * y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5 * inversesqrt(max(1.0 - x * x, 1e-7)) - v;

    return cross(v1, v2) * theta_sintheta;
}

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
    bool behind = (dot(dir, lightNormal) < 0.0);

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
        sum = 0.0;
    }

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

void main() {
    vec3 mAlbedo = pow(uAlbedo * texture(albedoMap, fTexCoords).rgb, vec3(2.2));
    float mMetallic = uMetallic * texture(metallicMap, fTexCoords).r;
    float mRoughness = uRoughness * texture(roughnessMap, fTexCoords).r;
    float mAO = texture(aoMap, fTexCoords).r;

	vec3 mNormal = normalize(fNormal);
	vec3 toCamera = normalize(uCameraPos - fWorldPos);
    vec3 reflected = reflect(-toCamera, mNormal);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, mAlbedo, mMetallic);

    float dotNV = max(dot(mNormal, toCamera), 0.0f);

    vec2 lut_uv = vec2(mRoughness, sqrt(1.0f - dotNV));
    lut_uv = lut_uv * LUT_SCALE + LUT_BIAS;
    vec4 t1 = texture(LTC1, lut_uv);
    vec4 t2 = texture(LTC2, lut_uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    vec3 areaLighting = vec3(0.0f);
	for (int i = 0; i < nAreaLights; i++) {
		vec3 diffuse = LTC_Evaluate(mNormal, toCamera, fWorldPos, mat3(1), areaLights[i].points, areaLights[i].twoSided);
		vec3 specular = LTC_Evaluate(mNormal, toCamera, fWorldPos, Minv, areaLights[i].points, areaLights[i].twoSided);
		specular *= specular * t2.x + (1.0f - specular) * t2.y;
		areaLighting += areaLights[i].emission * (specular + mAlbedo * diffuse);
	}

	vec3 pointLighting = vec3(0.0f);
	for(int i = 0; i < nPointLights; i++) {
		vec3 toLight = normalize(pointLights[i].position - fWorldPos);
		vec3 halfVector = normalize(toCamera + toLight);
        float dotNL = max(dot(mNormal, toLight), 0.0f);

		float distance = length(pointLights[i].position - fWorldPos);
        if(distance < 0.001f) continue;

		float attenuation = 1.0f / (distance * distance);
		vec3 radiance = pointLights[i].emission * attenuation;

		float NDF = DistributionGGX(mNormal, halfVector, mRoughness);
		float G = GeometrySmith(mNormal, toCamera, toLight, mRoughness);
		vec3 F = FresnelSchlick(max(dot(halfVector, toCamera), 0.0f), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0f * dotNV * dotNL + 0.0001f;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0f) - kS;
		kD *= 1.0f - mMetallic;

		pointLighting += (kD * mAlbedo / PI + specular) * radiance * dotNL;
	}

    vec3 F = FresnelSchlickRoughness(dotNV, F0, mRoughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - mMetallic;	  

    vec3 irradiance = texture(irradianceMap, mNormal).rgb;
    vec3 diffuse = irradiance * mAlbedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflected,  mRoughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUTMap, vec2(dotNV, mRoughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular);// * mAO;

	vec3 color = ambient + pointLighting + areaLighting;

	color = color / (color + vec3(1.0f));
	color = pow(color, vec3(1.0f / 2.2f));

	FragColor = vec4(color, 1.0f);
}
