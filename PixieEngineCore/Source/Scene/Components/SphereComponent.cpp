#include "pch.h"
#include "SphereComponent.h"
#include "GlobalRenderer.h"

SphereComponent::SphereComponent(SceneObject* parent, Float radius) :
	Component(ComponentType::Sphere, parent), m_radius(radius) {}

void SphereComponent::Draw() const {
	GlobalRenderer::DrawMesh(ResourceManager::GetSphereMesh());
}

Float SphereComponent::GetRadius() const {
	return m_radius;
}

void SphereComponent::SetRadius(Float radius) {
	m_radius = radius;
}
