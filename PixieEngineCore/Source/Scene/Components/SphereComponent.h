#pragma once
#include "pch.h"
#include "Component.h"

class SphereComponent : public Component {
public:
	SphereComponent(SceneObject* parent, float radius);

	void Draw() const;

	float GetRadius() const;
	void SetRadius(float radius);

protected:
	float m_radius;
};
