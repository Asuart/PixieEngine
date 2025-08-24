#include "pch.h"
#include "Mesh.h"

namespace PixieEngine {

Vertex::Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv) :
	position(p), normal(n), uv(uv) {
	std::fill(boneIDs.begin(), boneIDs.end(), -1);
	std::fill(boneWeights.begin(), boneWeights.end(), 0.0f);
}

bool Vertex::AddWeight(int32_t boneID, float weight, bool overrideSmallest) {
	if (weight == 0) return false;

	for (uint32_t i = 0; i < cMaxBonesPerVertex; i++) {
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
	float min = 0.0f;
	for (int32_t i = 0; i < cMaxBonesPerVertex; i++) {
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
}

glm::vec3 Mesh::GetCenter() const {
	glm::vec3 center = glm::vec3(0);
	for (size_t i = 0; i < m_vertices.size(); i++) {
		center += m_vertices[i].position;
	}
	if (m_vertices.size() > 1) {
		center /= (float)m_vertices.size();
	}
	return center;
}

Bounds3f Mesh::GetBounds() const {
	glm::vec3 min = glm::vec3(INFINITY);
	glm::vec3 max = glm::vec3(-INFINITY);
	for (const Vertex& v : m_vertices) {
		min = glm::min(min, v.position);
		max = glm::max(max, v.position);
	}
	return Bounds3f(min, max);
}

}