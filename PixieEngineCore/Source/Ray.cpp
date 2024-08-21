#include "pch.h"
#include "Ray.h"

Ray::Ray(uint32_t x, uint32_t y, const Vec3& origin, const Vec3& direction, Medium* medium)
	: x(x), y(y), origin(origin), direction(direction), medium(medium) {
	assert(!isnan(origin) || !isnan(direction));
}

Vec3 Ray::At(Float t) const {
	return origin + direction * t;
}