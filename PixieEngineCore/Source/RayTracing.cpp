#include "pch.h"
#include "RayTracing.h"
#include "Scene.h"
#include "SceneSnapshot.h"
#include "PathIntegrator.h"
#include "SimplePathIntegrator.h"
#include "RandomWalkIntegrator.h"
#include "TestNormalsIntegrator.h"
#include "TestSamplerIntegrator.h"

std::string to_string(RayTracingVisualization mode) {
	switch (mode) {
	case RayTracingVisualization::Integration: return "Integration";
	case RayTracingVisualization::BoxChecksStatistics: return "Box Checks Statistics";
	case RayTracingVisualization::TriangleChecksStatistics: return "Triangle Checks Statistics";
	default: return "Undefined Visualization Mode";
	}
}

std::string to_string(RayTracingMode mode) {
	switch (mode) {
	case RayTracingMode::RandomWalk: return "Random Walk";
	case RayTracingMode::SimplePathTracing: return "Simple Path Tracing";
	case RayTracingMode::PathTracing: return "Path Tracing";
	case RayTracingMode::TestNormals: return "Test Normals";
	case RayTracingMode::TestSampler: return "Test Sampler";
	default: return "Undefined Ray Tracing Mode";
	}
}

std::optional<ShapeIntersection> RayTracing::Intersect(Ray ray, SceneSnapshot* sceneSnapshot, Float tMax) {
	return sceneSnapshot->GetRootPrimitive()->Intersect(ray, tMax);
}

bool RayTracing::IsIntersected(Ray ray, SceneSnapshot* sceneSnapshot, Float tMax) {
	return sceneSnapshot->GetRootPrimitive()->IsIntersected(ray, tMax);
}

std::optional<ShapeIntersection> RayTracing::Intersect(Ray ray, const Scene* scene, Float tMax) {
	return {};
}

bool RayTracing::IsIntersected(Ray ray, const Scene* scene, Float tMax) {
	return false;
}

bool RayTracing::IsIntersected(Ray ray, Bounds3f bounds, Float tMax, Float* hitt0, Float* hitt1) {
	RayTracingStatistics::IncrementBoxTests();
	if (bounds.IsDegenerate()) {
		return false;
	}
	Float t0 = 0, t1 = tMax;
	for (int32_t i = 0; i < 3; ++i) {
		Float tNear = (bounds.min[i] - ray.origin[i]) * ray.inverseDirection[i];
		Float tFar = (bounds.max[i] - ray.origin[i]) * ray.inverseDirection[i];
		if (tNear > tFar) {
			std::swap(tNear, tFar);
		}
		tFar *= 1 + 2 * gamma(3);

		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}
	if (hitt0) *hitt0 = t0;
	if (hitt1) *hitt1 = t1;
	return true;
}

bool RayTracing::IsIntersected(Ray ray, Bounds3f bounds, Float raytMax, const int32_t dirIsNeg[3]) {
	RayTracingStatistics::IncrementBoxTests();

	Float tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * ray.inverseDirection.x;
	Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * ray.inverseDirection.x;
	Float tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * ray.inverseDirection.y;
	Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * ray.inverseDirection.y;

	tMax *= 1 + 2 * gamma(3);
	tyMax *= 1 + 2 * gamma(3);

	if (tMin > tyMax || tyMin > tMax) {
		return false;
	}
	if (tyMin > tMin) {
		tMin = tyMin;
	}
	if (tyMax < tMax) {
		tMax = tyMax;
	}

	Float tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * ray.inverseDirection.z;
	Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * ray.inverseDirection.z;

	tzMax *= 1 + 2 * gamma(3);

	if (tMin > tzMax || tzMin > tMax) {
		return false;
	}
	if (tzMin > tMin) {
		tMin = tzMin;
	}
	if (tzMax < tMax) {
		tMax = tzMax;
	}

	return (tMin < raytMax) && (tMax > 0);
}

bool RayTracing::Unoccluded(SceneSnapshot* sceneSnapshot, const SurfaceInteraction& p0, const SurfaceInteraction& p1) {
	return Unoccluded(sceneSnapshot, p0.position, p1.position);
}

bool RayTracing::Unoccluded(SceneSnapshot* sceneSnapshot, const Vec3& p0, const Vec3& p1) {
	RayTracingStatistics::IncrementRays();
	Vec3 dir = p1 - p0;
	Float tMax = glm::length(dir);
	return !RayTracing::IsIntersected(Ray(p0, glm::normalize(dir)), sceneSnapshot, tMax - ShadowEpsilon);
}

RayTracer* CreateRayTracer(RayTracingMode mode) {
	switch (mode) {
	case RayTracingMode::SimplePathTracing:
		return new SimplePathIntegrator();
	case RayTracingMode::PathTracing:
		return new PathIntegrator();
	case RayTracingMode::TestNormals:
		return new TestNormalsIntegrator();
	case RayTracingMode::TestSampler:
		return new TestSamplerIntegrator();
	case RayTracingMode::RandomWalk:
	default:
		return new RandomWalkIntegrator();
	}
}
