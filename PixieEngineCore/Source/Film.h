#pragma once
#include "Texture.h"
#include "FilmFilters.h"

class Film {
public:
	Texture<glm::fvec4>* texture;
	glm::ivec2 m_resolution;
	Vec2 m_pixelSize;
	FilmFilter* m_filter;
	Float m_maxSampleBrightness = 128.0f;

	Film(glm::ivec2 resolution);

	void Reset();
	void Resize(glm::ivec2 resolution);
	void SetSample(int32_t x, int32_t y, Spectrum L, Float weight);
	void AddSample(int32_t x, int32_t y, Spectrum L, Float weight);

	Vec2 GetUV(int32_t x, int32_t y) const;
	Vec2 GetUV(int32_t x, int32_t y, const Vec2& u) const;
	Vec2 GetUV(Vec2 p) const;
	Vec2 GetUV(Vec2 p, const Vec2& u) const;

	FilmFilter* GetFilter();
};
