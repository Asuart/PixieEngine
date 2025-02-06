#pragma once
#include "pch.h"
#include "Math/Transform.h"
#include "Math/Frame.h"
#include "RayInteraction.h"
#include "Resources/Mesh.h"

struct ShapeSampleContext {
	Vec3 position;
	Vec3 normal;

	ShapeSampleContext(const Vec3& position, const Vec3& normal = Vec3(0)) :
		position(position), normal(normal) {}

	ShapeSampleContext(const RayInteraction& intr) :
		position(intr.position), normal(intr.normal) {}
};

struct ShapeSample {
	RayInteraction intr; // Sampled shape point.
	Float pdf; // Probability to sample point.

	ShapeSample(const RayInteraction& intr, Float p) :
		intr(intr), pdf(p) {}
};

struct ShapeIntersection {
	RayInteraction intr; // intersection point information.
	Float tHit; // distance from tay start to point

	ShapeIntersection(const RayInteraction& intr, Float t) :
		intr(intr), tHit(t) {}
};

class Shape {
public:
	int32_t m_materialIndex = 0;
	int32_t m_lightIndex = -1;

	Shape(int32_t materialIndex) :
		m_materialIndex(materialIndex) {}

	virtual Float Area() const = 0;
	virtual std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const = 0;
	virtual bool IsIntersected(const Ray& ray, Float tMax = Infinity) const = 0;
	virtual Bounds3f Bounds() const = 0;
	virtual std::optional<ShapeSample> Sample(Vec2 u) const = 0;
	virtual std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const = 0;
	virtual Float SamplePDF(const RayInteraction&) const = 0;
	virtual Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const = 0;
};

class Sphere : public Shape {
public:
	Sphere(int32_t material, const Transform& transform, Float r);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const RayInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;

protected:
	Transform m_transform;
	Float m_radius;
};

class Triangle : public Shape {
public:
	Vec3 p0, p1, p2;
	Vec2 uv0, uv1, uv2;
	Vec3 normal;

	Triangle(int32_t materialIndex, const Vertex& v0, const Vertex& v1, const Vertex& v2);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const RayInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;
	Float GetSolidAngle(const Vec3& point) const;
};
