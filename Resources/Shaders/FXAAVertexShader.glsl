#version 330 core

layout(location = 0) in vec2 position;

out vec2 fTexCoords;

void main() {
	fTexCoords = vec2(position.x, position.y);
	vec2 transformedPosition = vec2(position.x, position.y) * 2.0 - vec2(1.0, 1.0);
	gl_Position = vec4(transformedPosition, 0.0, 1.0);
}
