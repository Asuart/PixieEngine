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

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);
	~Mesh();

	Vec3 GetCenter() const;
	void Draw() const;
	void DrawWireframe() const;
	void Upload();
	void Bind() const;
	void FreeCPUData();
	void FreeGPUData();
	
	inline const std::vector<Vertex>& GetVertices() const { return m_vertices; }
	inline const std::vector<int32_t>& GetIndices() const { return m_indices; }

protected:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	int32_t m_indicesCount = 0;
};