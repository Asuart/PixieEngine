#include "pch.h"
#include "MeshComponent.h"
#include "GlobalRenderer.h"

MeshComponent::MeshComponent(SceneObject* parent, Mesh* mesh) :
	Component(ComponentType::Mesh, parent), m_mesh(mesh) {
	UploadMesh();
}

const Mesh* MeshComponent::GetMesh() const {
	return m_mesh;
}

Mesh* MeshComponent::GetMesh() {
	return m_mesh;
}

void MeshComponent::Draw() const {
	GlobalRenderer::DrawMesh(m_mesh);
}

void MeshComponent::UploadMesh() {
	m_mesh->Upload();
}
