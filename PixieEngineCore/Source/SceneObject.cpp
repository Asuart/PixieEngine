
#include "SceneObject.h"

SceneObject::SceneObject(const std::string& name, Transform transform)
	: name(name), transform(transform) {}

void SceneObject::AddComponent(Component* component) {
	components.push_back(component);
}

void SceneObject::RemoveComponent(Component* component) {
	for (size_t i = 0; i < components.size(); i++) {
		if (components[i] == component) {
			components.erase(components.begin() + i);
			break;
		}
	}
}

void SceneObject::RemoveComponent(const std::string& name) {
	for (size_t i = 0; i < components.size(); i++) {
		if (components[i]->name == name) {
			components.erase(components.begin() + i);
			break;
		}
	}
}

template<typename T>
Component* SceneObject::GetComponent() {
	for (size_t i = 0; i < components.size(); i++) {
		if (dynamic_cast<T*>(components[i])) {
			return components[i];
		}
	}
	return nullptr;
}

Component* SceneObject::GetComponent(const std::string& name) {
	for (size_t i = 0; i < components.size(); i++) {
		if (components[i]->name == name) {
			return components[i];
		}
	}
	return nullptr;
}

void SceneObject::OnStart() {}

void SceneObject::OnUpdate() {}

void SceneObject::OnFixedUpdate() {}