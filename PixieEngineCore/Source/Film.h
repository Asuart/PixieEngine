#pragma once
#include "ShaderLibrary.h"
#include "OGLMesh.h"

class Film {
public:
	OGLMesh* mesh;
	Texture<glm::vec4>* texture;
	glm::ivec2 m_resolution;
	Vec2 m_pixelSize;

	Film(glm::ivec2 resolution);

	void Reset();
	void Resize(glm::ivec2 resolution);
	void SetPixel(int32_t x, int32_t y, const glm::vec4& color);
	void AddPixel(int32_t x, int32_t y, const glm::vec4& color);

	Vec2 GetUV(int32_t x, int32_t y) const;
	Vec2 GetUV(int32_t x, int32_t y, const Vec2& u) const;
};
