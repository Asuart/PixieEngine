#version 420 core

in vec2 fTexCoords;
in vec3 fWorldPos;
in vec3 fNormal;

layout (location = 0) out vec4 oAccumulator;
layout (location = 1) out float oReveal;

uniform vec4 uColor;

void main() {
	float weight = clamp(pow(min(1.0, uColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	oAccumulator = vec4(uColor.rgb * uColor.a, uColor.a) * weight;
	oReveal = uColor.a;
}