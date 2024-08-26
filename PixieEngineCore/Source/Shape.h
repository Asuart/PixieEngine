#pragma once
#include "pch.h"
#include "Transform.h"
#include "Interaction.h"

struct ShapeSampleContext {
	Vec3 position;
	Vec3 normal;

	ShapeSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	ShapeSampleContext(const SurfaceInteraction& intr);
};

struct ShapeSample {
	SurfaceInteraction intr; // Sampled shape point.
	Float pdf; // Probability to sample point.

	ShapeSample(SurfaceInteraction intr, Float p);
};

struct ShapeIntersection {
	SurfaceInteraction intr; // intersection point information.
	Float tHit; // distance from tay start to point
};

class Shape {
public:
	const Transform& GetTransform() const;
	virtual Float Area() const = 0;
	virtual std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const = 0;
	virtual bool IntersectP(const Ray& ray, Float tMax = Infinity) const = 0;
	virtual Bounds3f Bounds() const = 0;
	virtual std::optional<ShapeSample> Sample(Vec2 u) const = 0;
	virtual std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const = 0;
	virtual Float SamplePDF(const SurfaceInteraction&) const = 0;
	virtual Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const = 0;

protected:
	Transform m_transform;
};
