#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Math/BezierCurves.h"

namespace PixieEngine {

class MeshGenerator {
public:
	static Mesh BezierMesh(const BezierCurve2D& curve, float width = 1.0f, uint32_t subdivisions = 32);
	static Mesh Quad(glm::vec2 min, glm::vec2 max);
	static Mesh Cube(glm::vec3 size = glm::vec3(1.0f));
	static Mesh SphereFromOctahedron(float radius, uint32_t subdivisions);
};

}