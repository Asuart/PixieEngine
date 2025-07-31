#version 330 core

out vec4 color;

in vec2 fTexCoord;

uniform sampler2D textureLeft;
uniform sampler2D textureRight;
uniform vec3 uBalance;
uniform vec3 uScale;

void main() {
	vec3 inverseBalance = vec3(1.0f, 1.0f, 1.0f) - uBalance;
	color = (texture(textureLeft, fTexCoord) * vec4(uBalance, 1.0f) + texture(textureRight, fTexCoord) * vec4(inverseBalance, 1.0f)) * vec4(uScale, 1.0f);
}