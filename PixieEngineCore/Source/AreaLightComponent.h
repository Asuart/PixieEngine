#pragma once
#include "Component.h"
#include "MeshComponent.h"

class AreaLightComponent : public Component {
public:
public:
	AreaLightComponent(SceneObject* parent, Vec3 color, Float strength);

	void OnStart() override;
	Vec3 GetEmission();
	Vec3 GetColor();
	Float GetStrength();
	void SetColor(Vec3 color);
	void SetStrength(Float strength);

protected:
	Mesh* m_mesh = nullptr;
	Vec3 m_color;
	Float m_strength;
};