#pragma once
#include "Scene/SceneObject.h"

class SceneObject;

enum class ComponentType : uint32_t {
	Camera,
	DiffuseAreaLight,
	DirectionalLight,
	Material,
	MeshAnimator,
	Mesh,
	PointLight,
	Sphere,
	SphereRigidbody
};

class Component {
public:
	Component() = delete;
	
	SceneObject* GetParent();

	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

protected:
	SceneObject* m_parent;
	ComponentType m_type;

	Component(ComponentType type, SceneObject* parent);

	friend class Scene;
};
