#pragma once

const char* const STEREOSCOPIC_QUAD_VERTEX_SHADER_SOURCE =
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

const char* const STEREOSCOPIC_QUAD_FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"out vec4 color;\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D textureLeft;\n"
"uniform sampler2D textureRight;\n"
"uniform vec3 uBalance;\n"
"uniform vec3 uScale;\n"
"void main() {\n"
"	vec3 inverseBalance = vec3(1.0f, 1.0f, 1.0f) - uBalance;"
"	color = (texture(textureLeft, fTexCoord) * vec4(uBalance, 1.0f) + texture(textureRight, fTexCoord) * vec4(inverseBalance, 1.0f)) * vec4(uScale, 1.0f);\n"
"}\n";
