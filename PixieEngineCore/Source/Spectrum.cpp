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

Spectrum Spectrum::operator*(const Float& value) {
	return Spectrum(m_rgb * value);
}

Spectrum Spectrum::operator*(const Spectrum& other) {
	return Spectrum(m_rgb * other.m_rgb);
}

void Spectrum::operator*=(const Float& value) {
	m_rgb *= value;
}

void Spectrum::operator*=(const Spectrum& other) {
	m_rgb *= other.m_rgb;
}

void Spectrum::operator/=(const Float& value) {
	m_rgb /= value;
}

void Spectrum::operator/=(const Spectrum& other) {
	m_rgb /= other.m_rgb;
}

bool Spectrum::operator==(const Spectrum& other) {
	return m_rgb == other.m_rgb;
}

bool Spectrum::operator==(const glm::fvec3& value) {
	return m_rgb == value;
}

bool Spectrum::operator!=(const Spectrum& other) {
	return m_rgb != other.m_rgb;
}

bool Spectrum::operator!=(const glm::fvec3& value) {
	return m_rgb != value;
}

Spectrum Spectrum::operator/(const Float& value) {
	return Spectrum(m_rgb / value);
}

Spectrum::operator bool() const {
	return m_rgb != glm::fvec3(0.0f, 0.0f, 0.0f);
}

Spectrum operator*(const Float& left, const Spectrum& right) {
	return Spectrum(right.m_rgb * left);
}

Spectrum operator*(const Spectrum& left, const Float& right) {
	return Spectrum(left.m_rgb * right);
}

Spectrum operator*(const Spectrum& left, const Spectrum& right) {
	return Spectrum(left.m_rgb * right.m_rgb);
}