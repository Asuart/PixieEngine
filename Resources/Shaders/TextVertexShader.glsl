#version 330 core

layout(location = 0) in vec2 position;

out vec2 fTexCoords;

uniform vec2 uPos;
uniform vec2 uSize;
uniform mat4 mProjection;

void main() {
	fTexCoords = vec2(position.x, position.y);
	vec2 transformedPosition = position * uSize + uPos;
	gl_Position = mProjection * vec4(transformedPosition, 0.0, 1.0);
} 
