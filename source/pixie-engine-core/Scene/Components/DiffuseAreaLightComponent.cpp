#include "pch.h"
#include "DiffuseAreaLightComponent.h"

namespace PixieEngine {

DiffuseAreaLightComponent::DiffuseAreaLightComponent(SceneObject* parent, MeshHandle meshHandle, Mesh mesh, glm::vec3 color, float strength) :
	Component(ComponentType::DiffuseAreaLight, parent), m_meshHandle(meshHandle), m_mesh(mesh), m_color(color), m_strength(strength) {
}

void DiffuseAreaLightComponent::OnStart() {}

glm::vec3 DiffuseAreaLightComponent::GetEmission() const {
	return m_color * m_strength;
}

glm::vec3 DiffuseAreaLightComponent::GetColor() const {
	return m_color;
}

void DiffuseAreaLightComponent::SetColor(glm::vec3 color) {
	m_color = color;
}

float DiffuseAreaLightComponent::GetStrength() const {
	return m_strength;
}

void DiffuseAreaLightComponent::SetStrength(float strength) {
	m_strength = strength;
}

MeshHandle DiffuseAreaLightComponent::GetMeshHandle() const {
	return m_meshHandle;
}

void DiffuseAreaLightComponent::SetMeshHandle(MeshHandle meshHandle) {
	m_meshHandle = meshHandle;
}

const Mesh& DiffuseAreaLightComponent::GetMesh() const {
	return m_mesh;
}

void DiffuseAreaLightComponent::SetMesh(const Mesh& mesh) {
	m_mesh = mesh;
}

}