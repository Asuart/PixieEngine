#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTMath.h"

struct Ray {
	Vec3 o;
	Vec3 d;

	Ray(const Vec3& _o = Vec3(0.0), const Vec3& _d = Vec3(0.0));

	Vec3 At(Float t) const;
};
