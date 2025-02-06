#pragma once
#include "pch.h"
#include "Math/MathBase.h"

struct Ray {
	Vec3 origin;
	Vec3 direction;
	Vec3 inverseDirection;

	Ray(const Vec3& origin, const Vec3& direction) :
		origin(origin), direction(direction), inverseDirection(1.0f / direction) {
		// check if ray is valid
		assert(!isnan(origin) || !isnan(direction));
		// check if direction is normalized
		assert(glm::abs(glm::length(direction) - 1.0f) < ShadowEpsilon);
	}

	Vec3 At(Float t) const;
	void Reflect(Vec3 normal);
	void Refract(Vec3 normal, Float eta);
	void SkipIntersection(Vec3 position);
};

inline Vec3 Ray::At(Float t) const {
	return origin + direction * t;
}

inline void Ray::Reflect(Vec3 normal) {
	direction = glm::reflect(direction, normal);
	inverseDirection = 1.0f / direction;
}

inline void Ray::Refract(Vec3 normal, Float eta) {
	direction = glm::refract(direction, normal, eta);
	inverseDirection = 1.0f / direction;
}

inline void Ray::SkipIntersection(Vec3 position) {
	origin = position + direction * ShadowEpsilon;
}
