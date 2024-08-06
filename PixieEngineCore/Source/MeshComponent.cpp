#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, const SceneObject* parent)
	: Component("Mesh Component", parent), mesh(mesh) {}

const Mesh* MeshComponent::GetMesh() const {
	return mesh;
}