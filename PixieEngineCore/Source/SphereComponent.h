#pragma once
#include "pch.h"
#include "Component.h"

class SphereComponent : public Component {
public:
	SphereComponent(SceneObject* parent, Float radius);

	Float GetRadius();
	void SetRadius(Float radius);

protected:
	Float m_radius;
};