#pragma once
#include "pch.h"
#include "RTMath.h"

struct Medium;

struct Ray {
	uint32_t x, y;
	Vec3 origin;
	Vec3 direction;
	Medium* medium;

	Ray(uint32_t x, uint32_t y, const Vec3& origin, const Vec3& direction, Medium* medium = nullptr);

	Vec3 At(Float t) const;
};