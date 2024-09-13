#include "pch.h"
#include "Bounds.h"

Bounds2i::Bounds2i(glm::ivec2 p) 
	: min(p), max(p) {}

Bounds2i::Bounds2i(glm::ivec2 p1, glm::ivec2 p2) 
	: min(glm::min(p1, p2)), max(glm::max(p1, p2)) {}

glm::ivec2 Bounds2i::Diagonal() const {
	return max - min;
}

glm::ivec2 Bounds2i::Center() const {
	return min + Diagonal() / 2;
}

Float Bounds2i::Area() const {
	glm::ivec2 diagonal = Diagonal();
	return (Float)diagonal.x * diagonal.y;
}

int32_t Bounds2i::MaxDimension() const {
	glm::ivec2 diagonal = Diagonal();
	return diagonal.x >= diagonal.y ? 0 : 1;
}

glm::ivec2 Bounds2i::Lerp(Float p) const {
	return glm::ivec2(::Lerp(p, min.x, max.x), ::Lerp(p, min.y, max.y));
}

glm::ivec2 Bounds2i::Lerp(Vec2 p) const {
	return glm::ivec2(::Lerp(p.x, min.x, max.x), ::Lerp(p.y, min.y, max.y));
}

void Bounds2i::BoundingCircle(Vec2* center, Float* radius) const {
	*center = Center();
	*radius = glm::length(Vec2(Diagonal()) / 2.0f);
}

bool Bounds2i::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y;
}

bool Bounds2i::IsDegenerate() const {
	return min.x > max.x || min.y > max.y;
}

glm::ivec2 Bounds2i::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::ivec2& Bounds2i::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

bool Bounds2i::operator==(const Bounds2i& b) const {
	return min == b.min && max == b.max;
}

bool Bounds2i::operator!=(const Bounds2i& b) const {
	return min != b.min || max != b.max;
}

Bounds2f::Bounds2f(Vec2 p) 
	: min(p), max(p) {}

Bounds2f::Bounds2f(Vec2 p1, Vec2 p2) 
	: min(glm::min(p1, p2)), max(glm::max(p1, p2)) {}

Vec2 Bounds2f::Diagonal() const {
	return max - min;
}

Vec2 Bounds2f::Center() const {
	return min + Diagonal() / 2.0f;
}

Float Bounds2f::Area() const {
	Vec2 diagonal = Diagonal();
	return diagonal.x * diagonal.y;
}

int32_t Bounds2f::MaxDimension() const {
	Vec2 diagonal = Diagonal();
	return diagonal.x >= diagonal.y ? 0 : 1;
}

Vec2 Bounds2f::Lerp(Float p) const {
	return Vec2(::Lerp(p, min.x, max.x), ::Lerp(p, min.y, max.y));
}

Vec2 Bounds2f::Lerp(Vec2 p) const {
	return Vec2(::Lerp(p.x, min.x, max.x), ::Lerp(p.y, min.y, max.y));
}

Vec2 Bounds2f::Offset(Vec2 p) const {
	Vec2 o = p - min;
	if (max.x > min.x) {
		o.x /= max.x - min.x;
	}
	if (max.y > min.y) {
		o.y /= max.y - min.y;
	}
	return o;
}

void Bounds2f::BoundingCircle(Vec2* center, Float* radius) const {
	*center = Center();
	*radius = glm::length(Vec2(Diagonal()) / 2.0f);
}

bool Bounds2f::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y;
}

bool Bounds2f::IsDegenerate() const {
	return min.x > max.x || min.y > max.y;
}

Vec2 Bounds2f::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

Vec2& Bounds2f::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

bool Bounds2f::operator==(const Bounds2f& b) const {
	return min == b.min && max == b.max;
}

bool Bounds2f::operator!=(const Bounds2f& b) const {
	return min != b.min || max != b.max;
}

Bounds3i::Bounds3i(glm::ivec3 p) 
	: min(p), max(p) {}

Bounds3i::Bounds3i(glm::ivec3 p1, glm::ivec3 p2)
	: min(glm::min(p1, p2)), max(glm::max(p1, p2)) {}

glm::ivec3 Bounds3i::Corner(int32_t corner) const {
	return glm::ivec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

glm::ivec3 Bounds3i::Diagonal() const {
	return max - min;
}

glm::ivec3 Bounds3i::Center() const {
	return min + Diagonal() / 2;
}

Float Bounds3i::Area() const {
	glm::ivec3 d = Diagonal();
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Float Bounds3i::Volume() const {
	glm::ivec3 d = Diagonal();
	return (Float)d.x * d.y * d.z;
}

int32_t Bounds3i::MaxDimension() const {
	glm::ivec3 d = Diagonal();
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

glm::ivec3 Bounds3i::Lerp(Float t) const {
	return glm::ivec3(::Lerp(t, min.x, max.x), ::Lerp(t, min.y, max.y), ::Lerp(t, min.z, max.z));
}

glm::ivec3 Bounds3i::Lerp(Vec3 t) const {
	return glm::ivec3(::Lerp(t.x, min.x, max.x), ::Lerp(t.y, min.y, max.y), ::Lerp(t.z, min.z, max.z));
}

glm::ivec3 Bounds3i::Offset(glm::ivec3 p) const {
	glm::ivec3 o = p - min;
	if (max.x > min.x) {
		o.x /= max.x - min.x;
	}
	if (max.y > min.y) {
		o.y /= max.y - min.y;
	}
	if (max.z > min.z) {
		o.z /= max.z - min.z;
	}
	return o;
}

void Bounds3i::BoundingSphere(Vec3* center, Float* radius) const {
	*center = (Vec3)(min + max) / (Float)2.0f;
	*radius = glm::length(((Vec3)min + (Vec3)max) / 2.0f);
}

bool Bounds3i::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y || min.z >= max.z;
}

bool Bounds3i::IsDegenerate() const {
	return min.x > max.x || min.y > max.y || min.z > max.z;
}

bool Bounds3i::operator==(const Bounds3i& b) const {
	return b.min == min && b.max == max;
}

bool Bounds3i::operator!=(const Bounds3i& b) const {
	return b.min != min || b.max != max;
}

glm::ivec3 Bounds3i::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

glm::ivec3& Bounds3i::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

Bounds3f::Bounds3f(Vec3 p)
	: min(p), max(p) {}

Bounds3f::Bounds3f(Vec3 p1, Vec3 p2)
	: min(glm::min(p1, p2)), max(glm::max(p1, p2)) {}

Vec3 Bounds3f::Corner(int32_t corner) const {
	return Vec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

Vec3 Bounds3f::Diagonal() const {
	return max - min;
}

Vec3 Bounds3f::Center() const {
	return min + Diagonal() * 0.5f;
}

Float Bounds3f::Area() const {
	Vec3 d = Diagonal();
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Float Bounds3f::Volume() const {
	Vec3 d = Diagonal();
	return d.x * d.y * d.z;
}

int32_t Bounds3f::MaxDimension() const {
	Vec3 d = Diagonal();
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

Vec3 Bounds3f::Lerp(Float t) const {
	return Vec3(::Lerp(t, min.x, max.x), ::Lerp(t, min.y, max.y), ::Lerp(t, min.z, max.z));
}

Vec3 Bounds3f::Lerp(Vec3 t) const {
	return Vec3(::Lerp(t.x, min.x, max.x), ::Lerp(t.y, min.y, max.y), ::Lerp(t.z, min.z, max.z));
}

Vec3 Bounds3f::Offset(Vec3 p) const {
	Vec3 o = p - min;
	if (max.x > min.x) {
		o.x /= max.x - min.x;
	}
	if (max.y > min.y) {
		o.y /= max.y - min.y;
	}
	if (max.z > min.z) {
		o.z /= max.z - min.z;
	}
	return o;
}

void Bounds3f::BoundingSphere(Vec3* center, Float* radius) const {
	*center = (min + max) / (Float)2.0f;
	*radius = Inside(*center, *this) ? glm::distance(*center, max) : 0;
}

bool Bounds3f::IsEmpty() const {
	return min.x >= max.x || min.y >= max.y || min.z >= max.z;
}

bool Bounds3f::IsDegenerate() const {
	return min.x > max.x || min.y > max.y || min.z > max.z;
}

bool Bounds3f::IntersectP(const Ray& ray, Float tMax, Float* hitt0, Float* hitt1) const {
	RayTracingStatistics::IncrementBoxTests();
	Float t0 = 0, t1 = tMax;
	for (int32_t i = 0; i < 3; ++i) {
		Float invRayDir = 1 / ray.direction[i];
		Float tNear = (min[i] - ray.origin[i]) * invRayDir;
		Float tFar = (max[i] - ray.origin[i]) * invRayDir;
		if (tNear > tFar)
			std::swap(tNear, tFar);
		tFar *= 1 + 2 * gamma(3);

		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}
	if (hitt0) *hitt0 = t0;
	if (hitt1) *hitt1 = t1;
	return true;
}

bool Bounds3f::IntersectP(const Ray& ray, Float raytMax, Vec3 invDir, const int32_t dirIsNeg[3]) const {
	RayTracingStatistics::IncrementBoxTests();
	const Bounds3f& bounds = *this;
	Float tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * invDir.x;
	Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * invDir.x;
	Float tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * invDir.y;
	Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * invDir.y;

	tMax *= 1 + 2 * gamma(3);
	tyMax *= 1 + 2 * gamma(3);

	if (tMin > tyMax || tyMin > tMax) return false;
	if (tyMin > tMin) tMin = tyMin;
	if (tyMax < tMax) tMax = tyMax;

	Float tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * invDir.z;
	Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * invDir.z;
	tzMax *= 1 + 2 * gamma(3);

	if (tMin > tzMax || tzMin > tMax) return false;
	if (tzMin > tMin) tMin = tzMin;
	if (tzMax < tMax) tMax = tzMax;

	return (tMin < raytMax) && (tMax > 0);
}

bool Bounds3f::operator==(const Bounds3f& b) const {
	return b.min == min && b.max == max;
}

bool Bounds3f::operator!=(const Bounds3f& b) const {
	return b.min != min || b.max != max;
}

Vec3 Bounds3f::operator[](int32_t i) const {
	return (i == 0) ? min : max;
}

Vec3& Bounds3f::operator[](int32_t i) {
	return (i == 0) ? min : max;
}

Bounds2f Union(const Bounds2f& b1, const Bounds2f& b2) {
	Bounds2f ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds2i Union(const Bounds2i& b1, const Bounds2i& b2) {
	Bounds2i ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2) {
	Bounds3f ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

Bounds3i Union(const Bounds3i& b1, const Bounds3i& b2) {
	Bounds3i ret;
	ret.min = glm::min(b1.min, b2.min);
	ret.max = glm::max(b1.max, b2.max);
	return ret;
}

bool Inside(Vec3 p, const Bounds3f& b) {
	return (p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y && p.y <= b.max.y && p.z >= b.min.z && p.z <= b.max.z);
}
