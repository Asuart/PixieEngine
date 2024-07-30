#ifndef PIXIE_ENGINE_COMPONENT
#define PIXIE_ENGINE_COMPONENT

#include "pch.h"
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
	Component(const std::string& name, SceneObject* parent);

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();
};

#endif // PIXIE_ENGINE_COMPONENT