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

	bool AddWeight(int32_t boneID, float weigt, bool overrideSmallest = true);
};

struct Mesh {
	std::vector<Vertex> m_vertices;
	std::vector<int32_t> m_indices;
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	int32_t m_indicesCount = 0;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);
	~Mesh();

	Vec3 GetCenter() const;
	void Upload();
	void FreeCPUData();
	void FreeGPUData();
};