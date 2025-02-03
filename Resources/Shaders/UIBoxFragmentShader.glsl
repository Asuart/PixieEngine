#version 330 core

out vec4 color;

in vec2 fTexCoord;

uniform float uBorderRadius;
uniform ivec2 uBoxPixelSize;
uniform vec4 uBaseColor;

float calcDistance(vec2 uv) {
    vec2 positionInQuadrant = abs(uv * 2.0f - 1.0f);
    vec2 extend = vec2(uBoxPixelSize) / 2.0f;
    vec2 coords = positionInQuadrant * (extend + uBorderRadius);
    vec2 delta = max(coords - extend, 0.0f);
    return length(delta);
}

void main() {
    float dist = calcDistance(fTexCoord);
    if (dist > uBorderRadius) {
        //discard;
    }
    color = uBaseColor;
}
