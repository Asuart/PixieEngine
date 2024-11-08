#pragma once 
#include "pch.h"
#include "RTMath.h"
#include "Mesh.h"
#include "Shape.h"

class Triangle : public Shape {
public:
	Vec3 p0, p1, p2;
	Vec3 normal;
	Vec2 uv0, uv1, uv2;
	Vec3 edge0, edge1, edge2;
	Float area;
	Float d;
	Float samplePDF;

	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const SurfaceInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;
	Float GetSolidAngle(const Vec3& point) const;
};

class CompactTriangle : public Shape {
public:
	Vec3 p0, p1, p2;
	Vec2 uv0, uv1, uv2;
	Vec3 normal;

	CompactTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	Float Area() const override;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;
	Bounds3f Bounds() const override;
	std::optional<ShapeSample> Sample(Vec2 u) const override;
	std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Vec2 u) const override;
	Float SamplePDF(const SurfaceInteraction&) const override;
	Float SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const override;
	Float GetSolidAngle(const Vec3& point) const;
};
