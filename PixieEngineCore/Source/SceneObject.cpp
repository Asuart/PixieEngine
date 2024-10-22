#include "pch.h"
#include "SceneObject.h"

SceneObject::SceneObject(const std::string& name, Transform transform)
	: name(name), transform(transform), parent(nullptr) {}

void SceneObject::AddChild(SceneObject* object) {
	object->Detach();
	object->parent = this;
	children.push_back(object);
}

void SceneObject::Detach() {
	if (parent) {
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end()) {
			parent->children.erase(it);
		}
		parent = nullptr;
	}
}

void SceneObject::Remove() {
	Detach();
	delete this;
}

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

SceneObject* SceneObject::FindObject(const std::string& objectName) {
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->name == objectName) {
			return children[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> SceneObject::FindObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->name == objectName) {
			objects.push_back(children[i]);
		}
		std::vector<SceneObject*> childrenFound = children[i]->FindObjects(objectName);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}

SceneObject* SceneObject::FindObjectWithComponent(const std::string& componentName) {
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->GetComponent(componentName)) {
			return children[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> SceneObject::FindObjectsWithComponent(const std::string& componentName) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->GetComponent(componentName)) {
			objects.push_back(children[i]);
		}
		std::vector<SceneObject*> childrenFound = children[i]->FindObjectsWithComponent(componentName);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}
