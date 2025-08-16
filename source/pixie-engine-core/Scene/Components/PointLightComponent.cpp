#include "pch.h"
#include "PointLightComponent.h"

namespace PixieEngine {

PointLightComponent::PointLightComponent(SceneObject* parent, glm::vec3 color, float strength) :
	Component(ComponentType::PointLight, parent), m_color(color), m_strength(strength) {
}

glm::vec3 PointLightComponent::GetEmission() const {
	return m_color * m_strength;
}

glm::vec3 PointLightComponent::GetColor() const {
	return m_color;
}

void PointLightComponent::SetColor(glm::vec3 color) {
	m_color = color;
}

float PointLightComponent::GetStrength() const {
	return m_strength;
}

void PointLightComponent::SetStrength(float strength) {
	m_strength = strength;
}

}