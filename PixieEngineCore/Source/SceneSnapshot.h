#pragma once
#include "pch.h"
#include "SceneObject.h"
#include "Primitive.h"
#include "Light.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "Triangle.h"
#include "DiffuseAreaLight.h"

class SceneSnapshot {
public:
	SceneSnapshot(Scene* scene, uint32_t maxPrimitivesPerLeaf = 6);
	~SceneSnapshot();

	const Primitive* GetRootPrimitive() const;
	Bounds3f GetBounds() const;
	std::vector<DiffuseAreaLight*>& GetAreaLights();
	std::vector<Light*>& GetInfiniteLights();

private:
	std::vector<DiffuseAreaLight*> m_areaLights;
	std::vector<Light*> m_infiniteLights;
	Primitive* m_rootPrimitive;
};
