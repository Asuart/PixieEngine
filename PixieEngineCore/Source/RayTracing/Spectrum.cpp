#include "pch.h"
#include "Spectrum.h"

Spectrum::Spectrum(Float rgb) :
	m_rgb(rgb, rgb, rgb) {}

Spectrum::Spectrum(Float r, Float g, Float b) :
	m_rgb(r, g, b) {}

Spectrum::Spectrum(const Vec3& rgb) :
	m_rgb(rgb) {}

void Spectrum::SetRGB(Vec3 rgb) {
	m_rgb = rgb;
}

Vec3& Spectrum::GetRGB() {
	return m_rgb;
}

Float Spectrum::Average() const {
	return (m_rgb.x + m_rgb.y + m_rgb.z) / 3.0f;
}

Float Spectrum::MaxComponent() const {
	return glm::max(m_rgb.x, m_rgb.y, m_rgb.z);
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

bool Spectrum::operator==(const Vec3& value) {
	return m_rgb == value;
}

bool Spectrum::operator!=(const Spectrum& other) {
	return m_rgb != other.m_rgb;
}

bool Spectrum::operator!=(const Vec3& value) {
	return m_rgb != value;
}

Spectrum Spectrum::operator/(const Float& value) {
	return Spectrum(m_rgb / value);
}

Spectrum::operator bool() const {
	return m_rgb != Vec3(0.0f);
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
