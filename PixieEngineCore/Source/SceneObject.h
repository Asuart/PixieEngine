#pragma once
#include "UID.h"
#include "Transform.h"
#include "Component.h"
#include "ComponentTypes.h"

class Component;

class SceneObject {
public:
	const UID id;

	SceneObject(const std::string& name, Transform transform = Transform());

	void OnStart();
	void OnUpdate();
	void OnFixedUpdate();

	const std::string GetName() const;
	void SetName(const std::string& name);
	std::vector<SceneObject*>& GetChildren();
	SceneObject* GetChild(int32_t index);
	void AddChild(SceneObject* object);
	void Detach();
	void Remove();
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
	void RemoveComponent(ComponentType type);
	std::vector<Component*>& GetComponents();
	Component* GetComponent(int32_t index);
	Component* GetComponent(ComponentType type);
	Transform& GetTransform();

	SceneObject* FindObject(const std::string& objectName);
	std::vector<SceneObject*> FindObjects(const std::string& objectName);
	SceneObject* FindObjectWithComponent(ComponentType type);
	std::vector<SceneObject*> FindObjectsWithComponent(ComponentType type);

protected:
	std::string m_name;
	Transform m_localTransform;
	Transform m_worldTransform;
	SceneObject* m_parent;
	std::vector<SceneObject*> m_children;
	std::vector<Component*> m_components;

	friend class SceneManager;

public:
	template<typename T>
	T* GetComponent() {
		for (size_t i = 0; i < m_components.size(); i++) {
			T* cast = dynamic_cast<T*>(m_components[i]);
			if (cast) {
				return cast;
			}
		}
		return nullptr;
	}

	template<typename T>
	const T* GetComponent() const {
		for (size_t i = 0; i < m_components.size(); i++) {
			T* cast = dynamic_cast<T*>(m_components[i]);
			if (cast) {
				return cast;
			}
		}
		return nullptr;
	}
};
