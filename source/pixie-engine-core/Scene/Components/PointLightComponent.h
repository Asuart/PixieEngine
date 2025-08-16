#pragma once
#include "pch.h"
#include "Component.h"

namespace PixieEngine {

class PointLightComponent : public Component {
public:
	PointLightComponent(SceneObject* parent, glm::vec3 color, float strength);

	glm::vec3 GetEmission() const;
	glm::vec3 GetColor() const;
	void SetColor(glm::vec3 color);
	float GetStrength() const;
	void SetStrength(float strength);

protected:
	glm::vec3 m_color;
	float m_strength;
};

}