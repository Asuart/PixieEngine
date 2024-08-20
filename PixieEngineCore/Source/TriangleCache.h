#pragma once 
#include "pch.h"
#include "RTMath.h"
#include "Mesh.h"
#include "ShapeSample.h"

struct TriangleCache {
	Vec3 p0 = Vec3(0.0f, 0.0f, 0.0f), p1 = Vec3(0.0f, 0.0f, 0.0f), p2 = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 normal = Vec3(0.0f, 0.0f, 0.0f);
	Vec2 uv0 = Vec2(0.0f, 0.0f), uv1 = Vec2(0.0f, 0.0f), uv2 = Vec2(0.0f, 0.0f);
	Vec3 edge0 = Vec3(0.0f, 0.0f, 0.0f), edge1 = Vec3(0.0f, 0.0f, 0.0f), edge2 = Vec3(0.0f, 0.0f, 0.0f);
	Float area = 0.0f;
	Float d = 0.0f;
	Float samplePDF = 0.0f;

	TriangleCache(const Vertex& v0, const Vertex& v1, const Vertex& v2);

	Float GetSolidAngle(const Vec3& point) const;
	ShapeSample Sample(Vec2 u) const;
	ShapeSample Sample(const SurfaceInteraction& intr, Vec2 u) const;
};

Vec3 SampleUniformTriangle(Vec2 u);