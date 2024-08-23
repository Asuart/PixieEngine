#pragma once
#include "pch.h"
#include "Transform.h"

struct ShapeSampleContext {
	Vec3 position;
	Vec3 normal;

	ShapeSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	ShapeSampleContext(const SurfaceInteraction& intr);
};

struct ShapeSample {
	SurfaceInteraction intr; // Sampled point.
	Float p; // Probability to sample point.

	ShapeSample(SurfaceInteraction intr, Float p);
};

class Shape {
public:
	const Transform& GetTransform() const;
	Float Area();
	std::optional<ShapeSample> Sample(ShapeSampleContext context, Vec2 u);
	Float SamplePDF(ShapeSampleContext context, Vec3 wi);

protected:
	Transform m_transform;
};
