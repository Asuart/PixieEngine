#include "pch.h"
#include "MeshHandle.h"
#include "Rendering/RenderEngine.h"

std::map<GLuint, std::atomic<uint32_t>> MeshHandle::s_counters;

MeshHandle::MeshHandle(const Mesh& mesh) {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	UploadMesh(mesh);
	MeshHandle::s_counters[m_vao]++;
}

MeshHandle::MeshHandle(const MeshHandle& other) {
	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_ibo = other.m_ibo;
	m_indicesCount = other.m_indicesCount;
	MeshHandle::s_counters[m_vao]++;
}

MeshHandle::~MeshHandle() {
	if (!MeshHandle::s_counters.empty()) {
		MeshHandle::s_counters[m_vao]--;
		if (MeshHandle::s_counters[m_vao] == 0) {
			glDeleteVertexArrays(1, &m_vao);
			glDeleteBuffers(1, &m_vbo);
			glDeleteBuffers(1, &m_ibo);
		}
	}
}

MeshHandle& MeshHandle::operator= (const MeshHandle& other) {
	if (m_vao != other.m_vao) {
		MeshHandle::s_counters[m_vao]--;
		if (MeshHandle::s_counters[m_vao] == 0) {
			glDeleteVertexArrays(1, &m_vao);
			glDeleteBuffers(1, &m_vbo);
			glDeleteBuffers(1, &m_ibo);
		}
	}
	m_vao = other.m_vao;
	m_vbo = other.m_vbo;
	m_ibo = other.m_ibo;
	m_indicesCount = other.m_indicesCount;
	MeshHandle::s_counters[m_vao]++;
	return *this;
}

void MeshHandle::UploadMesh(const Mesh& mesh) {
	if (mesh.m_vertices.size() == 0 || mesh.m_indices.size() == 0) {
		return;
	}
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.m_vertices.size(), &mesh.m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * mesh.m_indices.size(), &mesh.m_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_indicesCount = (uint32_t)mesh.m_indices.size();
}

void MeshHandle::Draw() {
	RenderEngine::DrawMesh(*this);
}

uint32_t MeshHandle::GetIndicesCount() const {
	return m_indicesCount;
}
