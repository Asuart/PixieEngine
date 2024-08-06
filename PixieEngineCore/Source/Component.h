#pragma once
#include "UID.h"
#include "SceneObject.h"

class SceneObject;

class Component {
public:
	const UID id;
	const std::string name;
	const SceneObject* parent;

	Component() = delete;

protected:
	Component(const std::string& name, const SceneObject* parent);

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();
};