#include "pch.h"
#include "DirectionalLightComponent.h"

namespace PixieEngine {

DirectionalLightComponent::DirectionalLightComponent(SceneObject* parent, glm::vec3 direction, glm::vec3 color, float strength) :
	Component(ComponentType::DirectionalLight, parent), m_direction(glm::normalize(direction)), m_color(color), m_strength(strength) {
}

glm::vec3 DirectionalLightComponent::GetDirection() const {
	return m_direction;
}

void DirectionalLightComponent::SetDirection(glm::vec3 direction) {
	m_direction = glm::normalize(direction);
}

glm::vec3 DirectionalLightComponent::GetEmission() const {
	return m_color * m_strength;
}

glm::vec3 DirectionalLightComponent::GetColor() const {
	return m_color;
}

void DirectionalLightComponent::SetColor(glm::vec3 color) {
	m_color = color;
}

float DirectionalLightComponent::GetStrength() const {
	return m_strength;
}

void DirectionalLightComponent::SetStrength(float strength) {
	m_strength = strength;
}

}