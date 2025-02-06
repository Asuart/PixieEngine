#pragma once
#include "UID.h"
#include "Math/Transform.h"
#include "Component.h"
#include "ComponentTypes.h"

class Component;

class SceneObject {
public:
	const UID id;

	void OnStart();
	void OnUpdate();
	void OnFixedUpdate();

	const std::string GetName() const;
	void SetName(const std::string& name);
	const std::vector<SceneObject*>& GetChildren() const;
	SceneObject* GetChild(int32_t index) const;
	const std::vector<Component*>& GetComponents() const;
	Component* GetComponent(int32_t index) const;
	Component* GetComponent(ComponentType type) const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

	SceneObject* FindObject(const std::string& objectName) const;
	std::vector<SceneObject*> FindObjects(const std::string& objectName) const;
	SceneObject* FindObjectWithComponent(ComponentType type) const;
	std::vector<SceneObject*> FindObjectsWithComponent(ComponentType type) const;

protected:
	SceneObject(const std::string& name, Transform transform = Transform());

	std::string m_name;
	Transform m_localTransform;
	Transform m_worldTransform;
	SceneObject* m_parent;
	std::vector<SceneObject*> m_children;
	std::vector<Component*> m_components;

	friend class SceneManager;
	friend class Scene;

public:
	template<typename T>
	T* GetComponent() const {
		for (size_t i = 0; i < m_components.size(); i++) {
			T* cast = dynamic_cast<T*>(m_components[i]);
			if (cast) {
				return cast;
			}
		}
		return nullptr;
	}
};
