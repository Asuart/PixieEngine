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
