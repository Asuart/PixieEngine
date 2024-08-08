#pragma once

const char* const DEFAULT_VERTEX_SHADER_SOURCE =
"#version 330 core\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in vec3 norm;\n"
"layout(location = 2) in vec2 uv;\n"
"layout(location = 3) in ivec4 boneIDs;\n"
"layout(location = 4) in vec4 weights;\n"
"out vec3 fNormal;\n"
"uniform mat4 mModel;\n"
"uniform mat4 mView;\n"
"uniform mat4 mProjection;\n"
"const int MAX_BONES = 100;\n"
"const int MAX_BONE_INFLUENCE = 4;\n"
"uniform mat4 finalBonesMatrices[MAX_BONES];\n"
"void main()\n"
"{\n"
"    mat4 BoneTransform = finalBonesMatrices[boneIDs[0]] * weights[0];\n"
"    BoneTransform += finalBonesMatrices[boneIDs[1]] * weights[1];\n"
"    BoneTransform += finalBonesMatrices[boneIDs[2]] * weights[2];\n"
"    BoneTransform += finalBonesMatrices[boneIDs[3]] * weights[3];\n"
"    vec4 PosL = BoneTransform * vec4(pos, 1.0);\n"
"    gl_Position = mProjection * mView * PosL;\n"
"    vec4 totalPosition = vec4(0.0f);\n"
"    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {\n"
"        if (boneIDs[i] == -1)\n"
"            continue;\n"
"        if (boneIDs[i] >= MAX_BONES) {\n"
"            totalPosition = vec4(pos, 1.0f);\n"
"            break;\n"
"        }\n"
"        vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(pos, 1.0f);\n"
"        totalPosition += localPosition * weights[i];\n"
"        vec3 localNormal = mat3(finalBonesMatrices[boneIDs[i]]) * norm;\n"
"    }\n"
"    gl_Position = mProjection * mView * vec4(pos, 1.0);\n"
"    fNormal = norm;\n"
"}\n";

const char* const DEFAULT_FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"in vec3 fNormal;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"	float cos = abs(dot(fNormal, vec3(gl_FragCoord.x / 600.0, gl_FragCoord.y / 600.0, 1)));\n"
"	color = vec4(cos, cos, cos, 1.0) * 0.8 + vec4(1.0, 1.0, 1.0, 1.0) * 0.2;\n"
"}\n";