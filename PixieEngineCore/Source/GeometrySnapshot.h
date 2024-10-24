#pragma once
#include "pch.h"
#include "SceneObject.h"
#include "Primitive.h"
#include "Light.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TriangleCache.h"
#include "DiffuseAreaLight.h"

class GeometrySnapshot {
public:
	GeometrySnapshot(SceneObject* rootObject, uint32_t maxPrimitivesPerLeaf = 6);
	~GeometrySnapshot();

	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const;
	std::vector<DiffuseAreaLight*>& GetAreaLights();
	Bounds3f GetBounds();

private:
	std::vector<DiffuseAreaLight*> m_areaLights;
	Primitive* m_rootPrimitive;
};
