#pragma once
#include "pch.h"
#include "Component.h"

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent(SceneObject* parent, glm::vec3 direction, glm::vec3 color, float strength);

	glm::vec3 GetDirection() const;
	void SetDirection(glm::vec3 direction);
	glm::vec3 GetEmission() const;
	glm::vec3 GetColor() const;
	void SetColor(glm::vec3 color);
	float GetStrength() const;
	void SetStrength(float strength);

protected:
	glm::vec3 m_direction;
	glm::vec3 m_color;
	float m_strength;
};
