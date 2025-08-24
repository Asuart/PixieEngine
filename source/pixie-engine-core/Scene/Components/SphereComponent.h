#pragma once
#include "pch.h"
#include "Component.h"

namespace PixieEngine {

class SphereComponent : public Component {
public:
	SphereComponent(SceneObject* parent, float radius);

	float GetRadius() const;
	void SetRadius(float radius);

protected:
	float m_radius;
};

}