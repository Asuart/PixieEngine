#include "pch.h"
#include "MeshGenerator.h"
#include "Math/MathBase.h"

namespace PixieEngine {

Mesh MeshGenerator::BezierMesh(const BezierCurve2D& curve, float width, uint32_t subdivisions) {
	float hw = width * 0.5f;
	float stepsCount = (float)subdivisions + 1;
	float stepSize = 1.0f / stepsCount;

	std::vector<glm::vec2> points((int32_t)(stepsCount + 1));
	for (int32_t i = 0; i <= stepsCount; i++) {
		points[i] = curve.GetPoint(stepSize * i);
	}

	std::vector<glm::vec2> normals((int32_t)stepsCount);
	for (int32_t i = 1; i < stepsCount; i++) {
		glm::vec2 direction = glm::normalize(points[i + 1] - points[i]);
		normals[i] = glm::vec2(direction.y, -direction.x);
	}
	normals[0] = glm::normalize(curve.p1 - curve.p0);
	normals[0] = glm::vec2(normals[0].y, -normals[0].x);
	normals[normals.size() - 1] = glm::normalize(curve.p3 - curve.p2);
	normals[normals.size() - 1] = glm::vec2(normals[normals.size() - 1].y, -normals[normals.size() - 1].x);

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(points[0] + normals[0] * hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(points[0] + normals[0] * -hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f)));
	for (uint32_t i = 0; i < subdivisions; i++) {
		glm::vec2 midVector = glm::normalize(normals[i] + normals[i + 1]);
		vertices.push_back(Vertex(glm::vec3(points[i + 1] + midVector * hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(stepSize * i, 0.0f)));
		vertices.push_back(Vertex(glm::vec3(points[i + 1] + midVector * -hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(stepSize * i, 1.0f)));
	}
	vertices.push_back(Vertex(glm::vec3(points[points.size() - 1] + normals[normals.size() - 1] * hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f)));
	vertices.push_back(Vertex(glm::vec3(points[points.size() - 1] + normals[normals.size() - 1] * -hw, 0.0f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)));

	std::vector<int32_t> indices;
	for (int32_t i = 0; i < stepsCount * 2; i += 2) {
		indices.push_back(i + 0);
		indices.push_back(i + 3);
		indices.push_back(i + 1);
		indices.push_back(i + 0);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
	}

	return Mesh(vertices, indices);
}

Mesh MeshGenerator::Quad(glm::vec2 min, glm::vec2 max) {
	std::vector<Vertex> vertices{
		{glm::vec3(min.x, min.y, 0)},
		{glm::vec3(min.x, max.y, 0)},
		{glm::vec3(max.x, max.y, 0)},
		{glm::vec3(max.x, min.y, 0)},
	};

	std::vector<int32_t> indices{
		0, 1, 2,
		0, 2, 3
	};

	return Mesh(vertices, indices);
}

Mesh MeshGenerator::Cube(glm::vec3 size) {
	glm::vec3 hs = size / 2.0f;
	std::vector<Vertex> vertices{
		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(hs.x, -hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(hs.x,  hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(hs.x,  hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(0, 0, -1), glm::vec2(0.0f, 0.0f)),

		Vertex(glm::vec3(-hs.x, -hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(hs.x, -hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y,  hs.z), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)),

		Vertex(glm::vec3(-hs.x,  hs.y,  hs.z), glm::vec3(-1, 0, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y, -hs.z), glm::vec3(-1, 0, 0), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(-1, 0, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(-1, 0, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y,  hs.z), glm::vec3(-1, 0, 0), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y,  hs.z), glm::vec3(-1, 0, 0), glm::vec2(1.0f, 0.0f)),

		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(1, 0, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(hs.x,  hs.y, -hs.z), glm::vec3(1, 0, 0), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(hs.x, -hs.y, -hs.z), glm::vec3(1, 0, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(hs.x, -hs.y, -hs.z), glm::vec3(1, 0, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(hs.x, -hs.y,  hs.z), glm::vec3(1, 0, 0), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(1, 0, 0), glm::vec2(1.0f, 0.0f)),

		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(0, -1, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(hs.x, -hs.y, -hs.z), glm::vec3(0, -1, 0), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(hs.x, -hs.y,  hs.z), glm::vec3(0, -1, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(hs.x, -hs.y,  hs.z), glm::vec3(0, -1, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y,  hs.z), glm::vec3(0, -1, 0), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x, -hs.y, -hs.z), glm::vec3(0, -1, 0), glm::vec2(0.0f, 1.0f)),

		Vertex(glm::vec3(-hs.x,  hs.y, -hs.z), glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(hs.x,  hs.y, -hs.z), glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(hs.x,  hs.y,  hs.z), glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y,  hs.z), glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(-hs.x,  hs.y, -hs.z), glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f))
	};

	std::vector<int32_t> indices{
		 0,  1,  2,  3,  4,  5,
		 6,  7,  8,  9, 10, 11,
		12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35
	};

	return Mesh(vertices, indices);
}

Mesh MeshGenerator::SphereFromOctahedron(float radius, uint32_t subdivisions) {
	if (subdivisions > 10) subdivisions = 10;

	std::vector<Vertex> vertices{
		{glm::vec3(0, 1, 0)},
		{glm::vec3(1, 0, 0)},
		{glm::vec3(0, 0, -1)},
		{glm::vec3(-1, 0, 0)},
		{glm::vec3(0, 0, 1)},
		{glm::vec3(0, -1, 0)}
	};

	std::vector<int32_t> indices{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		5, 1, 2,
		5, 2, 3,
		5, 3, 4,
		5, 4, 1
	};

	std::vector<int32_t> newIndices;
	for (uint32_t subdivision = 0; subdivision < subdivisions; subdivision++) {
		for (size_t i = 0; i < indices.size(); i += 3) {
			vertices.push_back({ (vertices[indices[i + 0]].position + vertices[indices[i + 1]].position) / 2.0f });
			vertices.push_back({ (vertices[indices[i + 1]].position + vertices[indices[i + 2]].position) / 2.0f });
			vertices.push_back({ (vertices[indices[i + 2]].position + vertices[indices[i + 0]].position) / 2.0f });

			newIndices.push_back(indices[i + 0]);
			newIndices.push_back((int32_t)vertices.size() - 3);
			newIndices.push_back((int32_t)vertices.size() - 1);

			newIndices.push_back((int32_t)vertices.size() - 3);
			newIndices.push_back(indices[i + 1]);
			newIndices.push_back((int32_t)vertices.size() - 2);

			newIndices.push_back((int32_t)vertices.size() - 3);
			newIndices.push_back((int32_t)vertices.size() - 2);
			newIndices.push_back((int32_t)vertices.size() - 1);

			newIndices.push_back((int32_t)vertices.size() - 1);
			newIndices.push_back((int32_t)vertices.size() - 2);
			newIndices.push_back(indices[i + 2]);
		}
		indices = newIndices;
		newIndices.clear();
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		glm::vec3 n = glm::normalize(vertices[i].position);
		vertices[i].position = n * radius;
		vertices[i].normal = n;
		vertices[i].uv = glm::vec2(0.5f + (float)std::atan2(n.x, n.z) / TwoPi, n.y);
	}

	return Mesh(vertices, indices);
}

}