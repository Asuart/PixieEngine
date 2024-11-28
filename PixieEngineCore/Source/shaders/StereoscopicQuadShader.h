#pragma once

static const char* const STEREOSCOPIC_QUAD_VERTEX_SHADER_SOURCE = R"(
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
)";

static const char* const STEREOSCOPIC_QUAD_FRAGMENT_SHADER_SOURCE = R"(
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
)";
