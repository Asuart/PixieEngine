#version 330 core

layout(location = 0) in vec2 position;

out vec2 fTexCoord;

uniform vec2 uPos;
uniform vec2 uSize;

void main() {
	fTexCoord = vec2(position.x, 1.0 - position.y);
	vec2 transformedPosition = vec2(position.x * uSize.x + uPos.x, -position.y * uSize.y - uPos.y) * 2.0 - vec2(1.0, -1.0);
	gl_Position = vec4(transformedPosition, 0.0, 1.0);
}
