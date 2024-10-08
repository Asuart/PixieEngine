#pragma once
#include "UID.h"
#include "SceneObject.h"

class SceneObject;

class Component {
public:
	const UID id;
	const std::string name;
	SceneObject* parent;

	Component() = delete;

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

protected:
	Component(const std::string& name, SceneObject* parent);
};