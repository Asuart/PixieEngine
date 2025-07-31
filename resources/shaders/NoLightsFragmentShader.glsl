#version 330 core

out vec4 FragColor;

in vec2 fTexCoords;
in vec3 fNormal;
in vec3 fWorldPos;

uniform vec3 uCameraPos;
uniform vec3 uColor;

void main() {
    vec3 toCamera = normalize(fWorldPos - uCameraPos);
    vec3 color = uColor * 0.25 + uColor * abs(dot(toCamera, fNormal)) * 0.75;
	FragColor = vec4(color, 1.0f);
}
