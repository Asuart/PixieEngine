#pragma once
#include "pch.h"

static const uint32_t MaxBonesPerVertex = 4;

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	int32_t boneIDs[MaxBonesPerVertex];
	Float boneWeights[MaxBonesPerVertex];
	Vertex(const Vec3& p = Vec3(0), const Vec3& n = Vec3(0), const Vec2& uv = Vec2(0));
};

struct Mesh {
	const std::vector<Vertex> vertices;
	const std::vector<int32_t> indices;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);

	glm::fvec3 GetCenter() const;
};