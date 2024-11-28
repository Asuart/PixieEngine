#include "pch.h"
#include "SphereComponent.h"

SphereComponent::SphereComponent(SceneObject* parent, Float radius) :
	Component(ComponentType::Sphere, parent), m_radius(radius) {}

Float SphereComponent::GetRadius() {
	return m_radius;
}

void SphereComponent::SetRadius(Float radius) {
	m_radius = radius;
}
