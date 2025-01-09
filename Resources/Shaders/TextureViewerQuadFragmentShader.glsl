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
