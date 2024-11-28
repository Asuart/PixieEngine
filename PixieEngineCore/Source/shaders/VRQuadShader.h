#pragma once

static const char* const VR_QUAD_VERTEX_SHADER_SOURCE = R"(
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

static const char* const VR_QUAD_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 color;

in vec2 fTexCoord;

uniform sampler2D textureLeft;
uniform sampler2D textureRight;
uniform float uDistance;
uniform float uK;

void main() {
	if(fTexCoord.x < 0.5f) {
		vec2 distUV = vec2(fTexCoord.x * 2.0f - 0.5f, fTexCoord.y - 0.5f);
		float uva = atan(distUV.x, distUV.y);
		float uvd = sqrt(dot(distUV, distUV));
		uvd = uvd * (1.0 + uK * uvd * uvd);
		color = texture(textureLeft, vec2(0.5) + vec2(sin(uva), cos(uva)) * uvd);
	}
	else {
		vec2 distUV = vec2(fTexCoord.x * 2.0f - 1.5f, fTexCoord.y - 0.5f);
		float uva = atan(distUV.x, distUV.y);
		float uvd = sqrt(dot(distUV, distUV));
		uvd = uvd * (1.0 + uK * uvd * uvd);
		color = texture(textureRight, vec2(0.5) + vec2(sin(uva), cos(uva)) * uvd);
	}
}
)";
