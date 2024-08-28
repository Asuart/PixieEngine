#pragma once
#include "ShaderLibrary.h"
#include "OGLMesh.h"
#include "Texture.h"

class Film {
public:
	OGLMesh* mesh;
	Texture<glm::vec4>* texture;
	glm::ivec2 m_resolution;
	Vec2 m_pixelSize;
	Float m_maxSampleBrightness = 128.0f;

	Film(glm::ivec2 resolution);

	void Reset();
	void Resize(glm::ivec2 resolution);
	void SetPixel(int32_t x, int32_t y, glm::fvec3 color);
	void AddPixel(int32_t x, int32_t y, glm::fvec3 color);

	Vec2 GetUV(int32_t x, int32_t y) const;
	Vec2 GetUV(int32_t x, int32_t y, const Vec2& u) const;
};
