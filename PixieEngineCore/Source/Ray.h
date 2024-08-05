#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTMath.h"

class Medium;

struct Ray {
	Vec3 origin;
	Vec3 direction;
	Medium* medium;

	Ray(const Vec3& o = Vec3(0.0), const Vec3& f = Vec3(0.0));

	Vec3 At(Float t) const;
};