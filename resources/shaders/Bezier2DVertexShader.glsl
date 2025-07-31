#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec2 fTexCoords;

uniform mat4 mProjection;

void main() {
    fTexCoords = uv;
	gl_Position = mProjection * vec4(pos.xy, 0.0, 1.0);
}