#include "pch.h"
#include "SoftbodyComponent.h"

SoftbodyComponent::SoftbodyComponent(SceneObject* parent) :
	Component(ComponentType::Softbody, parent) {}

void SoftbodyComponent::OnStart() {
	m_mesh = m_parent->GetComponent<MeshComponent>()->GetMesh();
	m_positions.resize(m_mesh->m_vertices.size());
	m_lastPositions.resize(m_mesh->m_vertices.size());
	m_forces.resize(m_mesh->m_vertices.size());
	for (size_t i = 0; i < m_mesh->m_vertices.size(); i++) {
		m_positions[i] = m_mesh->m_vertices[i].position;
		m_lastPositions[i] = m_mesh->m_vertices[i].position;
		m_forces[i] = glm::fvec3(0);
	}

	m_springStarts.resize(m_mesh->m_indices.size());
	m_springEnds.resize(m_mesh->m_indices.size());
	m_springLengths.resize(m_mesh->m_indices.size());
	for (size_t i = 0; i < m_mesh->m_indices.size(); i += 3) {
		m_springStarts[i + 0] = m_mesh->m_indices[i + 0];
		m_springEnds[i + 0] = m_mesh->m_indices[i + 1];
		m_springLengths[i + 0] = glm::length(m_mesh->m_vertices[m_mesh->m_indices[i + 0]].position - m_mesh->m_vertices[m_mesh->m_indices[i + 1]].position);

		m_springStarts[i + 1] = m_mesh->m_indices[i + 1];
		m_springEnds[i + 1] = m_mesh->m_indices[i + 2];
		m_springLengths[i + 1] = glm::length(m_mesh->m_vertices[m_mesh->m_indices[i + 1]].position - m_mesh->m_vertices[m_mesh->m_indices[i + 2]].position);

		m_springStarts[i + 2] = m_mesh->m_indices[i + 2];
		m_springEnds[i + 2] = m_mesh->m_indices[i + 0];
		m_springLengths[i + 2] = glm::length(m_mesh->m_vertices[m_mesh->m_indices[i + 2]].position - m_mesh->m_vertices[m_mesh->m_indices[i + 0]].position);
	}

	std::vector<std::vector<int32_t>> nodeColors;
	nodeColors.resize(m_positions.size());
	std::vector<int32_t> springColors;
	springColors.resize(m_springStarts.size());
	for (size_t i = 0; i < m_springStarts.size(); i++) {
		int32_t color = 0;
		std::vector<int32_t>& start = nodeColors[m_springStarts[i]];
		std::vector<int32_t>& end = nodeColors[m_springEnds[i]];
		while ((std::find(start.begin(), start.end(), color) != start.end()) || (std::find(end.begin(), end.end(), color) != end.end())) {
			color++;
		}
		start.push_back(color);
		end.push_back(color);
		springColors[i] = color;
	}

	std::vector<int32_t> indices;
	indices.resize(springColors.size());
	for (size_t i = 0; i < indices.size(); i++) {
		indices[i] = (int32_t)i;
	}
	std::sort(indices.begin(), indices.end(), [&](int32_t first, int32_t second) {
		return springColors[first] < springColors[second];
		});
	std::vector<int32_t> springStartsCopy;
	springStartsCopy.resize(indices.size());
	std::vector<int32_t> springEndsCopy;
	springEndsCopy.resize(indices.size());
	std::vector<float> springLengthsCopy;
	springLengthsCopy.resize(indices.size());
	std::vector<int32_t> springColorsCopy;
	springColorsCopy.resize(indices.size());
	for (size_t i = 0; i < indices.size(); i++) {
		springStartsCopy[i] = m_springStarts[indices[i]];
		springEndsCopy[i] = m_springEnds[indices[i]];
		springLengthsCopy[i] = m_springLengths[indices[i]];
		springColorsCopy[i] = springColors[indices[i]];
	}
	m_springStarts = springStartsCopy;
	m_springEnds = springEndsCopy;
	m_springLengths = springLengthsCopy;
	springColors = springColorsCopy;
	springStartsCopy.clear();
	springEndsCopy.clear();
	springLengthsCopy.clear();
	springColorsCopy.clear();
}

void SoftbodyComponent::OnUpdate() {
	return;
	m_timeAligner += Time::deltaTime;
	while (m_timeAligner >= m_deltaTime) {
		m_timeAligner -= m_deltaTime;
		SetForcesToGravity();
		UpdatePositions();
	}
	UpdateMesh();
}

void SoftbodyComponent::SetForcesToGravity() {
	for (size_t i = 0; i < m_forces.size(); i++) {
		m_forces[i] = m_gravity;
	}
}

void SoftbodyComponent::UpdatePositions() {
	for (size_t i = 0; i < m_positions.size(); i++) {
		Vec3 velocity = m_positions[i] - m_lastPositions[i] + m_forces[i] * m_deltaTime;
		m_lastPositions[i] = m_positions[i];
		m_positions[i] += velocity;
	}
}

void SoftbodyComponent::UpdateMesh() {
	for (size_t i = 0; i < m_positions.size(); i++) {
		m_mesh->m_vertices[i].position = m_positions[i];
	}
	m_mesh->Upload();
}
