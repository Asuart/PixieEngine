
#include "Bounds.h"

Bounds2i::Bounds2i(glm::ivec2 p1, glm::ivec2 p2)
	: pMin(glm::min(p1, p2)), pMax(glm::max(p1, p2)) {}

Bounds3f::Bounds3f() {
	constexpr Float minNum = std::numeric_limits<Float>::lowest();
	constexpr Float maxNum = std::numeric_limits<Float>::max();
	pMin = Vec3(maxNum, maxNum, maxNum);
	pMax = Vec3(minNum, minNum, minNum);
}

Bounds3f::Bounds3f(Vec3 p)
	: pMin(p), pMax(p) {}

Bounds3f::Bounds3f(Vec3 p1, Vec3 p2)
	: pMin(glm::min(p1, p2)), pMax(glm::max(p1, p2)) {}

Vec3 Bounds3f::Corner(int32_t corner) const {
	return Vec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

Vec3 Bounds3f::Diagonal() const {
	return pMax - pMin;
}

Float Bounds3f::SurfaceArea() const {
	Vec3 d = Diagonal();
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Float Bounds3f::Volume() const {
	Vec3 d = Diagonal();
	return d.x * d.y * d.z;
}

int Bounds3f::MaxDimension() const {
	Vec3 d = Diagonal();
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

Vec3 Bounds3f::Lerp(Vec3 t) const {
	return Vec3(::Lerp(t.x, pMin.x, pMax.x), ::Lerp(t.y, pMin.y, pMax.y), ::Lerp(t.z, pMin.z, pMax.z));
}

Vec3 Bounds3f::Offset(Vec3 p) const {
	Vec3 o = p - pMin;
	if (pMax.x > pMin.x) {
		o.x /= pMax.x - pMin.x;
	}
	if (pMax.y > pMin.y) {
		o.y /= pMax.y - pMin.y;
	}
	if (pMax.z > pMin.z) {
		o.z /= pMax.z - pMin.z;
	}
	return o;
}

void Bounds3f::BoundingSphere(Vec3* center, Float* radius) const {
	*center = (pMin + pMax) / (Float)2.0f;
	*radius = Inside(*center, *this) ? glm::distance(*center, pMax) : 0;
}

bool Bounds3f::IsEmpty() const {
	return pMin.x >= pMax.x || pMin.y >= pMax.y || pMin.z >= pMax.z;
}

bool Bounds3f::IsDegenerate() const {
	return pMin.x > pMax.x || pMin.y > pMax.y || pMin.z > pMax.z;
}

bool Bounds3f::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax, Float* hitt0, Float* hitt1) const {
	stats.m_boxCheckStatBuffer.Increment(ray.x, ray.y);

	Float t0 = 0, t1 = tMax;
	for (int32_t i = 0; i < 3; ++i) {
		Float invRayDir = 1 / ray.direction[i];
		Float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
		Float tFar = (pMax[i] - ray.origin[i]) * invRayDir;
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

bool Bounds3f::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float raytMax, Vec3 invDir, const int32_t dirIsNeg[3]) const {
	stats.m_boxCheckStatBuffer.Increment(ray.x, ray.y);
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
	return b.pMin == pMin && b.pMax == pMax;
}

bool Bounds3f::operator!=(const Bounds3f& b) const {
	return b.pMin != pMin || b.pMax != pMax;
}

Vec3 Bounds3f::operator[](int32_t i) const {
	return (i == 0) ? pMin : pMax;
}

Vec3 Bounds3f::operator[](int32_t i) {
	return (i == 0) ? pMin : pMax;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2) {
	Bounds3f ret;
	ret.pMin = glm::min(b1.pMin, b2.pMin);
	ret.pMax = glm::max(b1.pMax, b2.pMax);
	return ret;
}

bool Inside(Vec3 p, const Bounds3f& b) {
	return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}