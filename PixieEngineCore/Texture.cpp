#include "pch.h"
#include "Texture.h"

template <>
void Texture<glm::vec4>::TexImage2D() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
}