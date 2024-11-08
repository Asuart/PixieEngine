#pragma once
#include "pch.h"
#include "Component.h"

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent(SceneObject* parent, Vec3 direction, Vec3 color, Float strength);

	Vec3 GetDirection();
	Vec3 GetEmission();
	Vec3 GetColor();
	Float GetStrength();
	void SetDirection(Vec3 direction);
	void SetColor(Vec3 color);
	void SetStrength(Float strength);

protected:
	Vec3 m_direction;
	Vec3 m_color;
	Float m_strength;
};
