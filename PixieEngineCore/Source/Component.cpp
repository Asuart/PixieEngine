#include "pch.h"
#include "Component.h"

Component::Component(const std::string& name, SceneObject* parent)
	: name(name), parent(parent) {}

void Component::OnStart() {}

void Component::OnUpdate() {}

void Component::OnFixedUpdate() {}