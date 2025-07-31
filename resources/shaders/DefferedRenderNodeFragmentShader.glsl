#version 330 core
layout (location = 0) out vec3 gAlbedo;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gPosition;
layout (location = 3) out float gSpecular;
layout (location = 4) out float gMetallic;
layout (location = 5) out float gRoughness;

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fNormal;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform sampler2D albedoTexture;

const float gamma = 2.2;

vec3 PowVec3(vec3 v, float p) {
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec3 ToLinear(vec3 v) { 
    return PowVec3(v, gamma);
}

void main() {    
    gPosition = fWorldPos;
    gRoughness = roughness;

    gNormal = normalize(fNormal);
    gMetallic = metallic;

    float mSpecular = pow(0.23f, gamma);
	vec3 mDiffuse = albedo * texture(albedoTexture, fTexCoords).xyz;
    gAlbedo = mDiffuse;
    gSpecular = mSpecular;
}  
