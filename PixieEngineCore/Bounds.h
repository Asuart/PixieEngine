#ifndef PIXIE_ENGINE_CORE_BOUNDS
#define PIXIE_ENGINE_CORE_BOUNDS

#include "pch.h"
#include "Ray.h"
#include "Math.h"

struct Bounds2i {
	glm::ivec2 pMin, pMax;
	Bounds2i(glm::ivec2 p1, glm::ivec2 p2);
};

class Bounds3f {
public:
	glm::vec3 pMin, pMax;

	Bounds3f();
	explicit Bounds3f(glm::vec3 p);
	Bounds3f(glm::vec3 p1, glm::vec3 p2);

	glm::vec3 Corner(int corner) const;
	glm::vec3 Diagonal() const;
	float SurfaceArea() const;
	float Volume() const;
	int MaxDimension() const;
	glm::vec3 Lerp(glm::vec3 t) const;
	glm::vec3 Offset(glm::vec3 p) const;
	void BoundingSphere(glm::vec3* center, float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;
	bool IntersectP(glm::vec3 o, glm::vec3 d, float tMax = Infinity, float* hitt0 = nullptr, float* hitt1 = nullptr) const;
	bool IntersectP(glm::vec3 o, glm::vec3 d, float tMax, glm::vec3 invDir, const int dirIsNeg[3]) const;

	glm::vec3 operator[](int i) const;
	glm::vec3 operator[](int i);
	bool operator==(const Bounds3f& b) const;
	bool operator!=(const Bounds3f& b) const;
};

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2);
bool Inside(glm::vec3 p, const Bounds3f& b);

#endif // PIXIE_ENGINE_CORE_BOUNDS