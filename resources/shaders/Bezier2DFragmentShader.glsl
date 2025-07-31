#version 330 core

in vec2 fTexCoords;

out vec4 color;

void main() {
    color = vec4(fTexCoords, 1.0f, 1.0f);
}
