#pragma once
#include "pch.h"

struct Spectrum {
	Spectrum(Float r = 0.0f, Float g = 0.0f, Float b = 0.0f);
	Spectrum(const Vec3& rgb);

	void SetRGB(Vec3 rgb);
	Vec3& GetRGB();
	const Vec3& GetRGBValue() const;
	Float Average() const;

	Spectrum& operator=(const Spectrum& other);
	Spectrum operator+(const Spectrum& other);
	void operator+=(const Spectrum& other);
	Spectrum operator*(const Float& value);
	Spectrum operator*(const Spectrum& value);
	void operator*=(const Float& value);
	void operator*=(const Spectrum& value);
	void operator/=(const Float& value);
	void operator/=(const Spectrum& value);
	bool operator==(const Spectrum& value);
	bool operator==(const Vec3& value);
	bool operator!=(const Spectrum& value);
	bool operator!=(const Vec3& value);
	Spectrum operator/(const Float& value);
	operator bool() const;

protected:
	Vec3 m_rgb;

	friend Spectrum operator*(const Float& left, const Spectrum& right);
	friend Spectrum operator*(const Spectrum& left, const Float& right);
	friend Spectrum operator*(const Spectrum& left, const Spectrum& right);
};

Spectrum operator*(const Float& left, const Spectrum& right);
Spectrum operator*(const Spectrum& left, const Float& right);
Spectrum operator*(const Spectrum& left, const Spectrum& right);
