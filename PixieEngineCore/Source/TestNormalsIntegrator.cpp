#include "pch.h"
#include "TestNormalsIntegrator.h"

Spectrum TestNormalsIntegrator::SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) {
	RayTracingStatistics::IncrementRays();
	std::optional<ShapeIntersection> si = RayTracing::Intersect(ray, sceneSnapshot);
	if (!si) {
		return Spectrum();
	}
	return Spectrum(glm::abs(si->intr.normal));
}
