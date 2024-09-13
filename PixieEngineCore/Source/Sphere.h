#pragma once
#include "pch.h"
#include "Shape.h"

class Sphere : public Shape {
public:
	Sphere(const Transform& transform, Float r);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const SurfaceInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;

protected:
	Float m_radius;
};
