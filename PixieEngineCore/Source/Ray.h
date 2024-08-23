#pragma once
#include "pch.h"
#include "RTMath.h"

struct Medium;

struct Ray {
	Vec3 origin;
	Vec3 direction;
	Medium* medium;

	Ray(const Vec3& origin, const Vec3& direction, Medium* medium = nullptr);

	Vec3 At(Float t) const;
};