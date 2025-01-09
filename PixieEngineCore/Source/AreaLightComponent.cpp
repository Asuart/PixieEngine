#include "pch.h"
#include "AreaLightComponent.h"

AreaLightComponent::AreaLightComponent(SceneObject* parent, Vec3 color, Float strength) :
	Component(ComponentType::DiffuseAreaLight, parent), m_color(color), m_strength(strength) {}

void AreaLightComponent::OnStart() {
	if (MeshComponent* meshComponent = m_parent->GetComponent<MeshComponent>()) {
		m_mesh = meshComponent->GetMesh();
	}
}

Vec3 AreaLightComponent::GetEmission() {
	return m_color * m_strength;
}

Vec3 AreaLightComponent::GetColor() {
	return m_color;
}

Float AreaLightComponent::GetStrength() {
	return m_strength;
}

void AreaLightComponent::SetColor(Vec3 color) {
	m_color = color;
}

void AreaLightComponent::SetStrength(Float strength) {
	m_strength = strength;
}