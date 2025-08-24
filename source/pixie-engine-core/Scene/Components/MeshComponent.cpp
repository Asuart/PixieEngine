#include "pch.h"
#include "MeshComponent.h"
#include "Rendering/RenderEngine.h"

namespace PixieEngine {

MeshComponent::MeshComponent(SceneObject* parent, MeshHandle mesh) :
	Component(ComponentType::Mesh, parent), m_mesh(mesh) {
}

MeshHandle MeshComponent::GetMesh() const {
	return m_mesh;
}

void MeshComponent::SetMesh(MeshHandle mesh) {
	m_mesh = mesh;
}

}