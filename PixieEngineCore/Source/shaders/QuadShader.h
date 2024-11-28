#pragma once

static const char* const QUAD_VERTEX_SHADER_SOURCE = R"(
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

static const char* const QUAD_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 color;

in vec2 fTexCoord;

uniform float uSamples;
uniform sampler2D ourTexture;

void main() {
	vec4 pixel = texture(ourTexture, fTexCoord);
	color = vec4(0.f, 0.0f, 0.0f, 1.0f);
	if (pixel.a < 0.1) {
		discard;
	}
	else color = vec4(pixel.rgb / uSamples, 1.0);
}
)";
