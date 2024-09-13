#pragma once

const char* const QUAD_VERTEX_SHADER_SOURCE =
"#version 330 core\n"
"layout(location = 0) in vec2 position;\n"
"out vec2 fTexCoord;\n"
"uniform vec2 uPos;\n"
"uniform vec2 uSize;\n"
"void main() {\n"
"   fTexCoord = vec2(position.x, 1.0 - position.y);\n"
"	vec2 transformedPosition = vec2(position.x * uSize.x + uPos.x, -position.y * uSize.y - uPos.y) * 2.0 - vec2(1.0, -1.0);\n"
"   gl_Position = vec4(transformedPosition, 0.0, 1.0);\n"
"}\n";

const char* const QUAD_FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"out vec4 color;\n"
"in vec2 fTexCoord;\n"
"uniform float uSamples;\n"
"uniform sampler2D ourTexture;\n"
"void main() {\n"
"	vec4 pixel = texture(ourTexture, fTexCoord);\n"
"	if(pixel.a < 1) discard;\n"
"	else color = vec4(pixel.rgb / uSamples, 1.0);\n"
"}\n";
