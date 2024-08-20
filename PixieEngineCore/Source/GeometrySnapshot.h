#pragma once
#include "pch.h"
#include "SceneObject.h"
#include "Primitive.h"
#include "Light.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TriangleCache.h"

class GeometrySnapshot {
public:
	GeometrySnapshot(const std::vector<SceneObject*>& flatObjects, uint32_t maxPrimitivesPerLeaf = 4, bool cacheTriangles = true);
	~GeometrySnapshot();

	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const;

	std::vector<AreaLight*>& GetAreaLights();

private:
	std::vector<AreaLight*> m_areaLights;
	Primitive* m_rootPrimitive;
};