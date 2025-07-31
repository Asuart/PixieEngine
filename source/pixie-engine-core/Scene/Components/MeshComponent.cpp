#include "pch.h"
#include "MeshComponent.h"
#include "Rendering/RenderEngine.h"

MeshComponent::MeshComponent(SceneObject* parent, MeshHandle mesh) :
	Component(ComponentType::Mesh, parent), m_mesh(mesh) {}

MeshHandle MeshComponent::GetMesh() const {
	return m_mesh;
}

void MeshComponent::SetMesh(MeshHandle mesh) {
	m_mesh = mesh;
}

void MeshComponent::Draw() const {
	RenderEngine::DrawMesh(m_mesh);
}

uint32_t MeshComponent::GetIndicesCount() const {
	return m_mesh.GetIndicesCount();
}
