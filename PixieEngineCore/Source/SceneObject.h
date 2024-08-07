#pragma once
#include "UID.h"
#include "Transform.h"
#include "Component.h"

class Component;

class SceneObject {
public:
	const UID id;
	const std::string name;
	SceneObject* parent;
	Transform transform;
	std::vector<SceneObject*> children;
	std::vector<Component*> components;

	SceneObject(const std::string& name, Transform transform = Transform());

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);
	void RemoveComponent(const std::string& name);
	Component* GetComponent(const std::string& name);

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

public:
	template<typename T>
	T* GetComponent() {
		for (size_t i = 0; i < components.size(); i++) {
			T* cast = dynamic_cast<T*>(components[i]);
			if (cast) {
				return cast;
			}
		}
		return nullptr;
	}
};
