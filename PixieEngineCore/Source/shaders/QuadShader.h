#pragma once

const char* const QUAD_VERTEX_SHADER_SOURCE =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"out vec2 fTexCoord;\n"
"uniform Float aspect;"
"void main() {\n"
"	Float xScale = 1.0;\n"
"	Float yScale = 1.0;\n"
"	if(aspect > 1.0f) {\n"
"		yScale /= aspect;"
"	} else {\n"
"		xScale *= aspect;"
"	}\n"
"   fTexCoord = vec2(position.x + 1.0, position.y + 1.0) / 2.0;\n"
"	vec3 transformedPosition = vec3(position.x * xScale, position.y * yScale, position.z);"
"   gl_Position = vec4(transformedPosition, 1.0);\n"
"}";

const char* const QUAD_FRAGMENT_SHADER_SOURCE =
"#version 330 core\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D ourTexture;\n"
"out vec4 color;\n"
"void main() {\n"
"   //color = vec4(fTexCoord.x, fTexCoord.y, 1.0, 1.0);\n"
"   vec4 pixel = texture(ourTexture, fTexCoord);\n"
"	Float scale = 1.0f / pixel.a;\n"
"	vec4 normalizedPixel = vec4(sqrt(pixel.r * scale), sqrt(pixel.g * scale), sqrt(pixel.b * scale), 1.0f);\n"
"	color = clamp(normalizedPixel, 0.0f, 1.0f);\n"
"}";