#version 330 core

out vec2 fTexCoords;
out vec3 fNormal;
out vec3 fWorldPos;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 weights;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

void main() {
	fTexCoords = uv;
	fWorldPos = (mModel * vec4(pos, 1.0f)).xyz;
    fNormal = normalize(mat3(transpose(inverse(mModel))) * norm);
    gl_Position = mProjection * mView * vec4(fWorldPos, 1.0f);
}
