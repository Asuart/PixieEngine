#pragma once
#include "pch.h"
#include "Ray.h"
#include "RTMath.h"
#include "RayTracingStatistics.h"

struct Bounds2i {
	glm::ivec2 pMin, pMax;
	Bounds2i(glm::ivec2 p1, glm::ivec2 p2);
};

class Bounds3f {
public:
	Vec3 pMin, pMax;

	Bounds3f();
	explicit Bounds3f(Vec3 p);
	Bounds3f(Vec3 p1, Vec3 p2);

	Vec3 Corner(int32_t corner) const;
	Vec3 Diagonal() const;
	Vec3 Center() const;
	Float SurfaceArea() const;
	Float Volume() const;
	int MaxDimension() const;
	Vec3 Lerp(Vec3 t) const;
	Vec3 Offset(Vec3 p) const;
	void BoundingSphere(Vec3* center, Float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity, Float* hitt0 = nullptr, Float* hitt1 = nullptr) const;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax, Vec3 invDir, const int32_t dirIsNeg[3]) const;

	Vec3 operator[](int32_t i) const;
	Vec3 operator[](int32_t i);
	bool operator==(const Bounds3f& b) const;
	bool operator!=(const Bounds3f& b) const;
};

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2);
bool Inside(Vec3 p, const Bounds3f& b);
