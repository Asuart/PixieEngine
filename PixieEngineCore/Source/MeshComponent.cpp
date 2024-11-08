#include "pch.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, SceneObject* parent)
	: Component(ComponentType::Mesh, parent), m_mesh(mesh) {
	UploadMesh();
}

const Mesh* MeshComponent::GetMesh() const {
	return m_mesh;
}

Mesh* MeshComponent::GetMesh() {
	return m_mesh;
}

void MeshComponent::Draw() const {
	m_mesh->Draw();
}

void MeshComponent::UploadMesh() {
	m_mesh->Upload();
}
