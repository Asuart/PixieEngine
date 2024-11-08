#pragma once

const char* const VR_QUAD_VERTEX_SHADER_SOURCE =
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

const char* const VR_QUAD_FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"out vec4 color;\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D textureLeft;\n"
"uniform sampler2D textureRight;\n"
"uniform float uDistance;\n"
"uniform float uK;\n"
"void main() {\n"
"	if(fTexCoord.x < 0.5f) {\n"
"		vec2 distUV = vec2(fTexCoord.x * 2.0f - 0.5f, fTexCoord.y - 0.5f);\n"
"		float uva = atan(distUV.x, distUV.y);\n"
"		float uvd = sqrt(dot(distUV, distUV));\n"
"		uvd = uvd * (1.0 + uK * uvd * uvd);\n"
"		color = texture(textureLeft, vec2(0.5) + vec2(sin(uva), cos(uva)) * uvd);\n"
"	}\n"
"	else {\n"
"		vec2 distUV = vec2(fTexCoord.x * 2.0f - 1.5f, fTexCoord.y - 0.5f);\n"
"		float uva = atan(distUV.x, distUV.y);\n"
"		float uvd = sqrt(dot(distUV, distUV));\n"
"		uvd = uvd * (1.0 + uK * uvd * uvd);\n"
"		color = texture(textureRight, vec2(0.5) + vec2(sin(uva), cos(uva)) * uvd);\n"
"	}\n"
"}\n";
