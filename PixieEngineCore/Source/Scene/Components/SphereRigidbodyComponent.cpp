#include "pch.h"
#include "SphereRigidbodyComponent.h"

SphereRigidbodyComponent::SphereRigidbodyComponent(SceneObject* parent, float radius, bool activate)
	: Component(ComponentType::SphereRigidbody, parent) {
	m_id = PhysicsEngine::CreateSphereBody(m_parent->GetTransform().GetPosition(), radius, activate);
}

SphereRigidbodyComponent::~SphereRigidbodyComponent() {
	PhysicsEngine::DestroyBody(m_id);
}

void SphereRigidbodyComponent::OnUpdate() {
	m_parent->GetTransform().SetPosition(PhysicsEngine::GetBodyPosition(m_id));
}