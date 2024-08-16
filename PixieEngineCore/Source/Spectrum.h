#pragma once
#include "PixieEngineCoreHeaders.h"

struct Spectrum {
	Spectrum(float r = 0.0f, float g = 0.0f, float b = 0.0f);
	Spectrum(const glm::fvec3& rgb);

	glm::fvec3& GetRGB();
	const glm::fvec3& GetRGBValue() const;

	Spectrum& operator=(const Spectrum& other);
	Spectrum operator+(const Spectrum& other);
	void operator+=(const Spectrum& other);

protected:
	glm::fvec3 m_rgb;
};