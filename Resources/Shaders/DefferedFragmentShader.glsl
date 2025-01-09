#version 330 core
layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gPositionRoughness;
layout (location = 2) out vec4 gNormalMetallic;

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fNormal;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

uniform int useDiffuseMap;
uniform sampler2D albedoTexture;

const float gamma = 2.2;

vec3 PowVec3(vec3 v, float p) {
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec3 ToLinear(vec3 v) { 
    return PowVec3(v, gamma);
}

void main() {    
    gPositionRoughness.xyz = fWorldPos;
    gPositionRoughness.w = roughness;

    gNormalMetallic.xyz = normalize(fNormal);
    gNormalMetallic.w = metallic;

    float mSpecular = pow(0.23f, gamma);
	vec3 mDiffuse = albedo;
	if(useDiffuseMap == 1) {
		mDiffuse *= texture(albedoTexture, fTexCoords).xyz;
	}
    gAlbedoSpec.rgb = mDiffuse;
    gAlbedoSpec.a = mSpecular;
}  
