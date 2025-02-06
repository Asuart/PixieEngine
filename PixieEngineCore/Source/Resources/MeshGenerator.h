#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Math/BezierCurves.h"

class MeshGenerator {
public:
	static Mesh* BezierMesh(const BezierCurve2D& curve, Float width = 1.0f, uint32_t subdivisions = 32);
	static Mesh* Quad(Vec2 min, Vec2 max);
	static Mesh* Cube(Vec3 size = Vec3(1.0f));
	static Mesh* SphereFromOctahedron(Float radius, uint32_t subdivisions);
};