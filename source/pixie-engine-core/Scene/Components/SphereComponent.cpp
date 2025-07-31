#include "pch.h"
#include "SphereComponent.h"
#include "Rendering/RenderEngine.h"

SphereComponent::SphereComponent(SceneObject* parent, float radius) :
	Component(ComponentType::Sphere, parent), m_radius(radius) {}

void SphereComponent::Draw() const {
	RenderEngine::DrawSphere();
}

float SphereComponent::GetRadius() const {
	return m_radius;
}

void SphereComponent::SetRadius(float radius) {
	m_radius = radius;
}
