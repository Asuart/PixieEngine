#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<int32_t>& _indices) :
	m_vertices(_vertices), m_indices(_indices) {}

glm::vec3 Mesh::GetCenter() const {
	glm::vec3 center = glm::vec3(0);
	for (size_t i = 0; i < m_vertices.size(); i++) {
		center += m_vertices[i].position;
	}
	return m_vertices.size() > 1 ? center / (float)m_vertices.size() : center;
}
