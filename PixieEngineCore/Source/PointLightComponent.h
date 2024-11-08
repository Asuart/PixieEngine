#pragma once
#include "pch.h"
#include "Component.h"

class PointLightComponent : public Component {
public:
	PointLightComponent(SceneObject* parent, Vec3 color, Float strength);

	Vec3 GetEmission();
	Vec3 GetColor();
	Float GetStrength();
	void SetColor(Vec3 color);
	void SetStrength(Float strength);

protected:
	Vec3 m_color;
	Float m_strength;
};
