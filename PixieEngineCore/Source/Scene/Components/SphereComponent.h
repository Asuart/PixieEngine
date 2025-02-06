#pragma once
#include "pch.h"
#include "Component.h"

class SphereComponent : public Component {
public:
	SphereComponent(SceneObject* parent, Float radius);

	void Draw() const;

	Float GetRadius() const;
	void SetRadius(Float radius);

protected:
	Float m_radius;
};