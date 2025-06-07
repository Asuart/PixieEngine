#pragma once
#include "pch.h"
#include "Component.h"
#include "Physics/PhysicsEngine.h"

class SphereRigidbodyComponent : public Component {
public:
	SphereRigidbodyComponent(SceneObject* parent, float radius, bool activate);
	~SphereRigidbodyComponent();

	void OnUpdate() override;

protected:
	uint32_t m_id = 0;
};
