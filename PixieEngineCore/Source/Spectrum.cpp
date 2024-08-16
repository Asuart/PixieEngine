#include "Spectrum.h"

Spectrum::Spectrum(float r, float g, float b) 
	: m_rgb(r, g, b) {}

Spectrum::Spectrum(const glm::fvec3& rgb)
	: m_rgb(rgb) {}

glm::fvec3& Spectrum::GetRGB() {
	return m_rgb;
}

const glm::fvec3& Spectrum::GetRGBValue() const {
	return m_rgb;
}

Spectrum& Spectrum::operator=(const Spectrum& other) {
	m_rgb = other.m_rgb;
	return *this;
}

Spectrum Spectrum::operator+(const Spectrum& other) {
	return Spectrum(m_rgb + other.m_rgb);
}

void Spectrum::operator+=(const Spectrum& other) {
	m_rgb += other.m_rgb;
}