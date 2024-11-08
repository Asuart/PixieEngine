#include "pch.h"
#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent(SceneObject* parent,Vec3 direction, Vec3 color, Float strength)
	: Component(ComponentType::DirectionalLight, parent), m_direction(glm::normalize(direction)), m_color(color), m_strength(strength) {}

Vec3 DirectionalLightComponent::GetDirection() {
	return m_direction;
}

Vec3 DirectionalLightComponent::GetEmission() {
	return m_color * m_strength;
}

Vec3 DirectionalLightComponent::GetColor() {
	return m_color;
}

Float DirectionalLightComponent::GetStrength() {
	return m_strength;
}

void DirectionalLightComponent::SetDirection(Vec3 direction) {
	m_direction = glm::normalize(direction);
}

void DirectionalLightComponent::SetColor(Vec3 color) {
	m_color = color;
}

void DirectionalLightComponent::SetStrength(Float strength) {
	m_strength = strength;
}
