#version 330 core

layout(location = 0) in vec2 position;

out vec2 fTexCoord;

uniform vec2 uPos;
uniform vec2 uSize;
uniform mat4 mProjection;

void main() {
	fTexCoord = vec2(position.x, 1.0 - position.y);
	vec2 transformedPosition = position * uSize + uPos;
	gl_Position = mProjection * vec4(transformedPosition, 0.0, 1.0);
}