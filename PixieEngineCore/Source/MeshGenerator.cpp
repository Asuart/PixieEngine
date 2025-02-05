#include "pch.h"
#include "MeshGenerator.h"

Mesh* MeshGenerator::BezierMesh(const BezierCurve2D& curve, Float width, uint32_t subdivisions) {
	Float hw = width * 0.5f;
	Float stepsCount = subdivisions + 1;
	Float stepSize = 1.0f / stepsCount;

	std::vector<Vec2> points((int32_t)(stepsCount + 1));
	for (int32_t i = 0; i <= stepsCount; i++) {
		points[i] = curve.GetPoint(stepSize * i);
	}

	std::vector<Vec2> normals((int32_t)stepsCount);
	for (int32_t i = 1; i < stepsCount; i++) {
		Vec2 direction = glm::normalize(points[i + 1] - points[i]);
		normals[i] = Vec2(direction.y, -direction.x);
	}
	normals[0] = glm::normalize(curve.p1 - curve.p0);
	normals[0] = Vec2(normals[0].y, -normals[0].x);
	normals[normals.size() - 1] = glm::normalize(curve.p3 - curve.p2);
	normals[normals.size() - 1] = Vec2(normals[normals.size() - 1].y, -normals[normals.size() - 1].x);

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vec3(points[0] + normals[0] * hw, 0.0f), Vec3(0, 0, 1), Vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(Vec3(points[0] + normals[0] * -hw, 0.0f), Vec3(0, 0, 1), Vec2(0.0f, 1.0f)));
	for (int32_t i = 0; i < subdivisions; i++) {
		Vec2 midVector = glm::normalize(normals[i] + normals[i + 1]);
		vertices.push_back(Vertex(Vec3(points[i + 1] + midVector * hw, 0.0f), Vec3(0, 0, 1), Vec2(stepSize * i, 0.0f)));
		vertices.push_back(Vertex(Vec3(points[i + 1] + midVector * -hw, 0.0f), Vec3(0, 0, 1), Vec2(stepSize * i, 1.0f)));
	}
	vertices.push_back(Vertex(Vec3(points[points.size() - 1] + normals[normals.size() - 1] * hw, 0.0f), Vec3(0, 0, 1), Vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(Vec3(points[points.size() - 1] + normals[normals.size() - 1] * -hw, 0.0f), Vec3(0, 0, 1), Vec2(1.0f, 1.0f)));

	std::vector<int32_t> indices;
	for (int32_t i = 0; i < stepsCount * 2; i += 2) {
		indices.push_back(i + 0);
		indices.push_back(i + 3);
		indices.push_back(i + 1);
		indices.push_back(i + 0);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
	}

	return new Mesh(vertices, indices);
}