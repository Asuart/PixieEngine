#pragma once

static const char* DEFAULT_VERTEX_SHADER_SOURCE = R"(
#version 330 core
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

out vec3 fNormal;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    vec4 totalPosition = vec4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (boneIDs[i] == -1) {
            continue;
        }
        if (boneIDs[i] >= MAX_BONES) {
            totalPosition = vec4(pos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(pos, 1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIDs[i]]) * norm;
    }
	mat4 viewModel = mView * mModel;
    gl_Position = mProjection * viewModel * totalPosition;
    fNormal = norm;
}
)";

static const char* const DEFAULT_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

in vec3 fNormal;
in vec4 fColor;
out vec4 color;

void main() {
	float cos = abs(dot(fNormal, vec3(gl_FragCoord.x / 600.0, gl_FragCoord.y / 600.0, 1)));
	color = vec4(cos, cos, cos, 1.0f) * 0.8f + vec4(1.0f, 1.0f, 1.0f, 1.0f) * 0.2f;
}
)";
