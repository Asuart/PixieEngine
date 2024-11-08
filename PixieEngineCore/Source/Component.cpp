#include "pch.h"
#include "Component.h"

Component::Component(ComponentType type, SceneObject* parent)
	: type(type), m_parent(parent) {}

SceneObject* Component::GetParent() {
	return m_parent;
}

void Component::OnStart() {}

void Component::OnUpdate() {}

void Component::OnFixedUpdate() {}
