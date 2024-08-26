#pragma once 
#include "pch.h"
#include "RTMath.h"
#include "Mesh.h"
#include "Shape.h"

class TriangleCache : public Shape {
public:
	TriangleCache(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const SurfaceInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;

protected:
	Vec3 p0 = Vec3(0.0f, 0.0f, 0.0f), p1 = Vec3(0.0f, 0.0f, 0.0f), p2 = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 normal = Vec3(0.0f, 0.0f, 0.0f);
	Vec2 uv0 = Vec2(0.0f, 0.0f), uv1 = Vec2(0.0f, 0.0f), uv2 = Vec2(0.0f, 0.0f);
	Vec3 edge0 = Vec3(0.0f, 0.0f, 0.0f), edge1 = Vec3(0.0f, 0.0f, 0.0f), edge2 = Vec3(0.0f, 0.0f, 0.0f);
	Float area = 0.0f;
	Float d = 0.0f;
	Float samplePDF = 0.0f;

	Float GetSolidAngle(const Vec3& point) const;

	friend class GeometrySnapshot;
};
