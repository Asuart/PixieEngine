#include "pch.h"
#include "SceneObject.h"

SceneObject::SceneObject(const std::string& name, Transform transform)
	: m_name(name), m_localTransform(transform), m_parent(nullptr) {}

void SceneObject::AddChild(SceneObject* object) {
	object->Detach();
	object->m_parent = this;
	m_children.push_back(object);
}

void SceneObject::Detach() {
	if (m_parent) {
		auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
		if (it != m_parent->m_children.end()) {
			m_parent->m_children.erase(it);
		}
		m_parent = nullptr;
	}
}

void SceneObject::Remove() {
	Detach();
	delete this;
}

void SceneObject::AddComponent(Component* component) {
	m_components.push_back(component);
}

void SceneObject::RemoveComponent(Component* component) {
	for (size_t i = 0; i < m_components.size(); i++) {
		if (m_components[i] == component) {
			m_components.erase(m_components.begin() + i);
			break;
		}
	}
}

void SceneObject::RemoveComponent(ComponentType type) {
	for (size_t i = 0; i < m_components.size(); i++) {
		if (m_components[i]->type == type) {
			m_components.erase(m_components.begin() + i);
			break;
		}
	}
}

std::vector<Component*>& SceneObject::GetComponents() {
	return m_components;
}

Component* SceneObject::GetComponent(int32_t index) {
	return m_components[index];
}

Component* SceneObject::GetComponent(ComponentType type) {
	for (size_t i = 0; i < m_components.size(); i++) {
		if (m_components[i]->type == type) {
			return m_components[i];
		}
	}
	return nullptr;
}

void SceneObject::OnStart() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnStart();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnStart();
	}
}

void SceneObject::OnUpdate() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnUpdate();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnUpdate();
	}
}

void SceneObject::OnFixedUpdate() {
	for (size_t i = 0; i < m_components.size(); i++) {
		m_components[i]->OnFixedUpdate();
	}
	for (size_t i = 0; i < m_children.size(); i++) {
		m_children[i]->OnFixedUpdate();
	}
}

const std::string SceneObject::GetName() const {
	return m_name;
}

void SceneObject::SetName(const std::string& name) {
	m_name = name;
}

Transform& SceneObject::GetTransform() {
	return m_localTransform;
}

std::vector<SceneObject*>& SceneObject::GetChildren() {
	return m_children;
}

SceneObject* SceneObject::GetChild(int32_t index) {
	return m_children[index];
}

SceneObject* SceneObject::FindObject(const std::string& objectName) {
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			return m_children[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> SceneObject::FindObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->m_name == objectName) {
			objects.push_back(m_children[i]);
		}
		std::vector<SceneObject*> childrenFound = m_children[i]->FindObjects(objectName);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}

SceneObject* SceneObject::FindObjectWithComponent(ComponentType type) {
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->GetComponent(type)) {
			return m_children[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> SceneObject::FindObjectsWithComponent(ComponentType type) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->GetComponent(type)) {
			objects.push_back(m_children[i]);
		}
		std::vector<SceneObject*> childrenFound = m_children[i]->FindObjectsWithComponent(type);
		objects.insert(objects.end(), childrenFound.begin(), childrenFound.end());
	}
	return objects;
}
