#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Shape.h"

class Scene;
class SceneSnapshot;

enum class RayTracingVisualization : uint32_t {
	Integration = 0,
	BoxChecksStatistics,
	TriangleChecksStatistics,
	COUNT
};

enum class RayTracingMode : uint32_t {
	RandomWalk = 0,
	SimplePathTracing,
	PathTracing,
	TestNormals,
	TestSampler,
	COUNT
};

std::string to_string(RayTracingVisualization mode);
std::string to_string(RayTracingMode mode);

namespace RayTracing {
	static const int32_t c_maxRayBounces = 4096;

	std::optional<ShapeIntersection> Intersect(Ray ray, SceneSnapshot* sceneSnapshot, Float tMax = Infinity);
	bool IsIntersected(Ray ray, SceneSnapshot* sceneSnapshot, Float tMax = Infinity);
	std::optional<ShapeIntersection> Intersect(Ray ray, const Scene* scene, Float tMax = Infinity);
	bool IsIntersected(Ray ray, const Scene* scene, Float tMax = Infinity);
	bool IsIntersected(Ray ray, Bounds3f bounds, Float tMax = Infinity, Float* hitt0 = nullptr, Float* hitt1 = nullptr);
	bool IsIntersected(Ray ray, Bounds3f bounds, Float tMax, const int32_t dirIsNeg[3]);
	bool Unoccluded(SceneSnapshot* sceneSnapshot, const SurfaceInteraction& p0, const SurfaceInteraction& p1);
	bool Unoccluded(SceneSnapshot* sceneSnapshot, const Vec3& p0, const Vec3& p1);
};

class RayTracer {
public:
	virtual void PreprocessSceneSnapshot(SceneSnapshot* sceneSnapshot) {};
	virtual Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) = 0;
};

RayTracer* CreateRayTracer(RayTracingMode mode);
