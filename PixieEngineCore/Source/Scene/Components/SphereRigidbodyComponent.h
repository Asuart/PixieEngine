#pragma once
#include "pch.h"
#include "Component.h"
#include "Physics/PhysicsEngine.h"

class SphereRigidbodyComponent : public Component {
public:
	SphereRigidbodyComponent(SceneObject* parent, Float radius, bool activate) :
		Component(ComponentType::SphereRigidbody, parent) {
		m_id = PhysicsEngine::CreateSphereBody(m_parent->GetTransform().GetPosition(), radius, activate);
	}

	void OnUpdate() override {
		m_parent->GetTransform().SetPosition(PhysicsEngine::GetBodyPosition(m_id));
	}
protected:
	uint32_t m_id = 0;
};
