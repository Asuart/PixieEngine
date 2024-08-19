#pragma once
#include "Ray.h"
#include "Bounds.h"
#include "ShapeSample.h"

Vec3 SampleUniformTriangle(Vec2 u);

struct SurfaceInteraction;

class Shape {
public:
	virtual Float SolidAngle(Vec3 p) const = 0;
	virtual bool IsValid() const = 0;
	virtual Float Area() const = 0;
	virtual ShapeSample Sample(Vec2 u) const = 0;
	virtual Bounds3f Bounds() const = 0;
	virtual bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const = 0;
	virtual bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const = 0;
	virtual Float PDF(SurfaceInteraction ctx, Vec3 wi) const = 0;
};

class Triangle : public Shape {
public:
	Vec3 v0, v1, v2;
	Vec3 n;

	Triangle(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _n = Vec3(0));

	Float SolidAngle(Vec3 p) const override;
	bool IsValid() const override;
	Float Area() const override;
	ShapeSample Sample(Vec2 u) const override;
	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const override;
	Float PDF(SurfaceInteraction ctx, Vec3 wi) const override;
};

class CachedTriangle : public Shape {
public:
	Vec3 v0, v1, v2;
	Vec3 edge0, edge1, edge2;
	Vec3 n;
	Float area;
	Float d;

	CachedTriangle(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _n = Vec3(0));

	Float SolidAngle(Vec3 p) const override;
	bool IsValid() const override;
	Float Area() const override;
	ShapeSample Sample(Vec2 u) const override;
	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const override;
	Float PDF(SurfaceInteraction ctx, Vec3 wi) const override;
};

class Sphere : public Shape {
public:
	Vec3 c;
	Float r;

	Sphere(Vec3 _c, Float _r);

	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const override;
};
