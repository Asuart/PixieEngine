#pragma once

static const char* const PBR_VERTEX_SHADER_SOURCE = R"(
#version 330 core

out vec2 fTexCoords;
out vec3 fWorldPos;
out vec3 fNormal;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
	mat4 localTransformMatrix = mat4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (boneIDs[i] == -1) {
			if(i == 0) {
				localTransformMatrix = mat4(1.0f);
				break;
			}
            continue;
		}
        if (boneIDs[i] >= MAX_BONES) {
            localTransformMatrix = mat4(1.0f);
            break;
        }
        localTransformMatrix += finalBonesMatrices[boneIDs[i]] * weights[i];
    }

	fTexCoords = uv;
	fWorldPos = (mModel * localTransformMatrix * vec4(pos, 1.0f)).xyz;
    fNormal = mat3(transpose(inverse(mModel * localTransformMatrix))) * norm;
    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
)";

static const char* const PBR_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 FragColor;

const int MAX_LIGHTS = 4;
const float PI = 3.14159265359;

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fNormal;

uniform vec3 cameraPos;
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ambientOcclusion;
uniform int useDiffuseMap;

uniform sampler2D albedoTexture;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

void main() {
	vec3 N = normalize(fNormal);
	vec3 V = normalize(cameraPos - fWorldPos);

	vec3 diffuseColor = albedo;
	if(useDiffuseMap == 1) {
		diffuseColor *= texture(albedoTexture, fTexCoords).xyz;
	}

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++) {
		vec3 L = normalize(lightPositions[i] - fWorldPos);
		vec3 H = normalize(V + L);

		float distance = length(lightPositions[i] - fWorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		vec3 F0 = vec3(0.04);
		F0 = mix(F0, diffuseColor, metallic);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * diffuseColor / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * diffuseColor * ambientOcclusion;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, 1.0);
}
)";