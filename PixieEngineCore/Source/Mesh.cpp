#include "pch.h"
#include "Mesh.h"

Vertex::Vertex(const Vec3& p, const Vec3& n, const Vec2& uv) :
	position(p), normal(n), uv(uv) {
	for (int32_t i = 0; i < MaxBonesPerVertex; i++) {
		boneIDs[i] = -1;
		boneWeights[i] = 0.0f;
	}
}

bool Vertex::AddWeight(int32_t boneID, float weight, bool overrideSmallest) {
	if (weight == 0) return false;

	for (uint32_t i = 0; i < MaxBonesPerVertex; i++) {
		if (boneIDs[i] == -1) {
			boneIDs[i] = boneID;
			boneWeights[i] = weight;
			return false;
		}
	}

	if (!overrideSmallest) {
		return false;
	}

	int32_t minIndex = 0;
	Float min = 0.0f;
	for (int32_t i = 0; i < MaxBonesPerVertex; i++) {
		if (boneWeights[i] < min) {
			min = boneWeights[i];
			minIndex = i;
		}
	}
	if (weight > min) {
		boneIDs[minIndex] = boneID;
		boneWeights[minIndex] = weight;
		return true;
	}
	return false;
}

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<int32_t>& _indices) :
	m_vertices(_vertices), m_indices(_indices) {
	Upload();
}

Mesh::~Mesh() {
	if(m_vao) glDeleteVertexArrays(1, &m_vao);
	if(m_vbo) glDeleteBuffers(1, &m_vbo);
	if(m_ibo) glDeleteBuffers(1, &m_ibo);
}

Vec3 Mesh::GetCenter() const {
	Vec3 min = Vec3(INFINITY);
	Vec3 max = Vec3(-INFINITY);
	for (size_t i = 0; i < m_vertices.size(); i++) {
		min = glm::min(min, m_vertices[i].position);
		max = glm::max(max, m_vertices[i].position);
	}
	return min + (max - min) / (Float)2;
}

void Mesh::Upload() {
	if (m_vertices.size() == 0 || m_indices.size() == 0) {
		return;
	}
	if (!m_vao) {
		glGenVertexArrays(1, &m_vao);
	}
	glBindVertexArray(m_vao);

	if (!m_vbo) {
		glGenBuffers(1, &m_vbo);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	if (!m_ibo) {
		glGenBuffers(1, &m_ibo);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_indicesCount = (uint32_t)m_indices.size();
}

void Mesh::FreeCPUData() {
	m_vertices.resize(0);
	m_indices.resize(0);
}

void Mesh::FreeGPUData() {
	glDeleteVertexArrays(1, &m_vao); m_vao = 0;
	glDeleteBuffers(1, &m_vbo); m_vbo = 0;
	glDeleteBuffers(1, &m_ibo); m_ibo = 0;
	m_indicesCount = 0;
}
