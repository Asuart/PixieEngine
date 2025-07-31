#version 420 core

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
    fNormal = normalize(mat3(transpose(inverse(mModel * localTransformMatrix))) * norm);
    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
