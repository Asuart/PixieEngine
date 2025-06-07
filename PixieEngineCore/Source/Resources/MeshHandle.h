#pragma once
#include "pch.h"
#include "Mesh.h"

class MeshHandle {
public:
	MeshHandle() = default;
	explicit MeshHandle(const Mesh& mesh);
	~MeshHandle();

	MeshHandle(const MeshHandle& other);
	MeshHandle& operator= (const MeshHandle& other);

	void UploadMesh(const Mesh& mesh);
	void Draw();

	uint32_t GetIndicesCount() const;

protected:
	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	uint32_t m_indicesCount = 0;

	static std::map<GLuint, std::atomic<uint32_t>> s_counters;

	friend class RenderEngine;
};
