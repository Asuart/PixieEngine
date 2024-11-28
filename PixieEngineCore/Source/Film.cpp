#include "pch.h"
#include "Film.h"

Film::Film(glm::ivec2 resolution) :
	m_resolution(resolution), m_pixelSize(Vec2(1.0f) / (Vec2)resolution) {
	texture = new Texture<glm::fvec4>(resolution);
	texture->Upload();
	m_filter = new GaussianFilter(Vec2(4.0), 0.85f);
}

void Film::Reset() {
	texture->Reset();
}

void Film::Resize(glm::ivec2 resolution) {
	m_resolution = resolution;
	m_pixelSize = Vec2(1.0f) / (Vec2)resolution;
	texture->Resize(resolution);
}

void Film::SetSample(int32_t x, int32_t y, Spectrum L, Float weight) {
	Vec3 rgb = L.GetRGB();
	Float max = MaxComponent(rgb);
	if (max > m_maxSampleBrightness) {
		rgb *= m_maxSampleBrightness / max;
	}
	texture->SetPixel(x, y, glm::fvec4(rgb * weight, weight));
}

void Film::AddSample(int32_t x, int32_t y, Spectrum L, Float weight) {
	Vec3 rgb = L.GetRGB();
	Float max = MaxComponent(rgb);
	if (max > m_maxSampleBrightness) {
		rgb *= m_maxSampleBrightness / max;
	}
	texture->AccumulatePixel(x, y, glm::fvec4(rgb * weight, weight));
}

Vec2 Film::GetUV(int32_t x, int32_t y) const {
	return Vec2((Float)x / m_resolution.x, (Float)y / m_resolution.y);
}

Vec2 Film::GetUV(int32_t x, int32_t y, const Vec2& u) const {
	return Vec2((Float)x / m_resolution.x, (Float)y / m_resolution.y) + m_pixelSize * u;
}

Vec2 Film::GetUV(Vec2 p) const {
	return Vec2(p.x / m_resolution.x, p.y / m_resolution.y);
}

Vec2 Film::GetUV(Vec2 p, const Vec2& u) const {
	return Vec2(p.x / m_resolution.x, p.y / m_resolution.y) + m_pixelSize * u;
}

FilmFilter* Film::GetFilter() {
	return m_filter;
}
