#include "pch.h"
#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(SceneObject* parent, Vec3 color, Float strength) :
	Component(ComponentType::PointLight, parent), m_color(color), m_strength(strength) {}

Vec3 PointLightComponent::GetEmission() {
	return m_color * m_strength;
}

Vec3 PointLightComponent::GetColor() {
	return m_color;
}

Float PointLightComponent::GetStrength() {
	return m_strength;
}

void PointLightComponent::SetColor(Vec3 color) {
	m_color = color;
}

void PointLightComponent::SetStrength(Float strength) {
	m_strength = strength;
}
