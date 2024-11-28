#include "pch.h"
#include "DiffuseAreaLightComponent.h"
#include "MeshComponent.h"

DiffuseAreaLightComponent::DiffuseAreaLightComponent(SceneObject* parent, Vec3 color, Float strength) :
	Component(ComponentType::DiffuseAreaLight, parent), m_color(color), m_strength(strength) {}

void DiffuseAreaLightComponent::OnStart() {
	if (MeshComponent* meshComponent = m_parent->GetComponent<MeshComponent>()) {
		m_mesh = meshComponent->GetMesh();
	}
}

Vec3 DiffuseAreaLightComponent::GetEmission() {
	return m_color * m_strength;
}

Vec3 DiffuseAreaLightComponent::GetColor() {
	return m_color;
}

Float DiffuseAreaLightComponent::GetStrength() {
	return m_strength;
}

void DiffuseAreaLightComponent::SetColor(Vec3 color) {
	m_color = color;
}

void DiffuseAreaLightComponent::SetStrength(Float strength) {
	m_strength = strength;
}
