#include "Bounds.h"

Bounds2i::Bounds2i(glm::ivec2 p1, glm::ivec2 p2)
	: pMin(glm::min(p1, p2)), pMax(glm::max(p1, p2)) {}

Bounds3f::Bounds3f() {
	constexpr float minNum = std::numeric_limits<float>::lowest();
	constexpr float maxNum = std::numeric_limits<float>::max();
	pMin = glm::vec3(maxNum, maxNum, maxNum);
	pMax = glm::vec3(minNum, minNum, minNum);
}

Bounds3f::Bounds3f(glm::vec3 p)
	: pMin(p), pMax(p) {}

Bounds3f::Bounds3f(glm::vec3 p1, glm::vec3 p2)
	: pMin(glm::min(p1, p2)), pMax(glm::max(p1, p2)) {}

glm::vec3 Bounds3f::Corner(int corner) const {
	return glm::vec3((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y, (*this)[(corner & 4) ? 1 : 0].z);
}

glm::vec3 Bounds3f::Diagonal() const {
	return pMax - pMin;
}

float Bounds3f::SurfaceArea() const {
	glm::vec3 d = Diagonal();
	return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float Bounds3f::Volume() const {
	glm::vec3 d = Diagonal();
	return d.x * d.y * d.z;
}

int Bounds3f::MaxDimension() const {
	glm::vec3 d = Diagonal();
	if (d.x > d.y && d.x > d.z)
		return 0;
	else if (d.y > d.z)
		return 1;
	else
		return 2;
}

glm::vec3 Bounds3f::Lerp(glm::vec3 t) const {
	return glm::vec3(::Lerp(t.x, pMin.x, pMax.x), ::Lerp(t.y, pMin.y, pMax.y), ::Lerp(t.z, pMin.z, pMax.z));
}

glm::vec3 Bounds3f::Offset(glm::vec3 p) const {
	glm::vec3 o = p - pMin;
	if (pMax.x > pMin.x)
		o.x /= pMax.x - pMin.x;
	if (pMax.y > pMin.y)
		o.y /= pMax.y - pMin.y;
	if (pMax.z > pMin.z)
		o.z /= pMax.z - pMin.z;
	return o;
}

void Bounds3f::BoundingSphere(glm::vec3* center, float* radius) const {
	*center = (pMin + pMax) / 2.0f;
	*radius = Inside(*center, *this) ? glm::distance(*center, pMax) : 0;
}

bool Bounds3f::IsEmpty() const {
	return pMin.x >= pMax.x || pMin.y >= pMax.y || pMin.z >= pMax.z;
}

bool Bounds3f::IsDegenerate() const {
	return pMin.x > pMax.x || pMin.y > pMax.y || pMin.z > pMax.z;
}

bool Bounds3f::IntersectP(glm::vec3 o, glm::vec3 d, float tMax, float* hitt0, float* hitt1) const {
	float t0 = 0, t1 = tMax;
	for (int i = 0; i < 3; ++i) {
		float invRayDir = 1 / d[i];
		float tNear = (pMin[i] - o[i]) * invRayDir;
		float tFar = (pMax[i] - o[i]) * invRayDir;
		if (tNear > tFar)
			std::swap(tNear, tFar);
		tFar *= 1 + 2 * gamma(3);

		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar < t1 ? tFar : t1;
		if (t0 > t1)
			return false;
	}
	if (hitt0)
		*hitt0 = t0;
	if (hitt1)
		*hitt1 = t1;
	return true;
}

bool Bounds3f::IntersectP(glm::vec3 o, glm::vec3 d, float raytMax, glm::vec3 invDir, const int dirIsNeg[3]) const {
	const Bounds3f& bounds = *this;
	float tMin = (bounds[dirIsNeg[0]].x - o.x) * invDir.x;
	float tMax = (bounds[1 - dirIsNeg[0]].x - o.x) * invDir.x;
	float tyMin = (bounds[dirIsNeg[1]].y - o.y) * invDir.y;
	float tyMax = (bounds[1 - dirIsNeg[1]].y - o.y) * invDir.y;

	tMax *= 1 + 2 * gamma(3);
	tyMax *= 1 + 2 * gamma(3);

	if (tMin > tyMax || tyMin > tMax)
		return false;
	if (tyMin > tMin)
		tMin = tyMin;
	if (tyMax < tMax)
		tMax = tyMax;

	float tzMin = (bounds[dirIsNeg[2]].z - o.z) * invDir.z;
	float tzMax = (bounds[1 - dirIsNeg[2]].z - o.z) * invDir.z;
	tzMax *= 1 + 2 * gamma(3);

	if (tMin > tzMax || tzMin > tMax)
		return false;
	if (tzMin > tMin)
		tMin = tzMin;
	if (tzMax < tMax)
		tMax = tzMax;

	return (tMin < raytMax) && (tMax > 0);
}

bool Bounds3f::operator==(const Bounds3f& b) const {
	return b.pMin == pMin && b.pMax == pMax;
}

bool Bounds3f::operator!=(const Bounds3f& b) const {
	return b.pMin != pMin || b.pMax != pMax;
}

glm::vec3 Bounds3f::operator[](int i) const {
	return (i == 0) ? pMin : pMax;
}

glm::vec3 Bounds3f::operator[](int i) {
	return (i == 0) ? pMin : pMax;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2) {
	Bounds3f ret;
	ret.pMin = glm::min(b1.pMin, b2.pMin);
	ret.pMax = glm::max(b1.pMax, b2.pMax);
	return ret;
}

bool Inside(glm::vec3 p, const Bounds3f& b) {
	return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}