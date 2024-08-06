#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTMath.h"

struct Medium;

struct Ray {
	uint32_t x, y;
	Vec3 origin;
	Vec3 direction;
	Medium* medium;

	Ray(uint32_t x, uint32_t y, const Vec3& o = Vec3(0.0), const Vec3& f = Vec3(0.0), Medium* medium = nullptr);

	Vec3 At(Float t) const;
};