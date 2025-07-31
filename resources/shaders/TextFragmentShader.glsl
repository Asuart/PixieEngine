#version 330 core

out vec4 color;

in vec2 fTexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, fTexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
} 
