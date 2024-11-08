#pragma once
#include "UID.h"
#include "SceneObject.h"
#include "ComponentTypes.h"

class SceneObject;

class Component {
public:
	const UID id;
	const ComponentType type;

	Component() = delete;
	
	SceneObject* GetParent();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

protected:
	Component(ComponentType type, SceneObject* parent);

	SceneObject* m_parent;

	friend class SceneManager;
};
