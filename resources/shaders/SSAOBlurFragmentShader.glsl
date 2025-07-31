#version 330 core
out float FragColor;

in vec2 fTexCoords;

uniform int uBlurRange;
uniform sampler2D inputTexture;

void main()  {
    vec2 texelSize = 1.0f / vec2(textureSize(inputTexture, 0));
    float result = 0.0f;
    for (int x = -uBlurRange; x <= uBlurRange; x++) {
        for (int y = -uBlurRange; y <= uBlurRange; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(inputTexture, fTexCoords + offset).r;
        }
    }
    FragColor = result / ((uBlurRange * 2 + 1) * (uBlurRange * 2 + 1));
}
