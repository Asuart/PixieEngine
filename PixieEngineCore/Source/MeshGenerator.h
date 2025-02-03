#pragma once
#include "pch.h"
#include "Mesh.h"
#include "BezierCurves.h"

class MeshGenerator {
public:
	static Mesh* BezierMesh(const BezierCurve2D& curve, Float width = 1.0f, uint32_t subdivisions = 32);
};