#include "pch.h"
#include "TestNormalsIntegrator.h"

TestNormalsIntegrator::TestNormalsIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum TestNormalsIntegrator::Integrate(Ray ray, Sampler* sampler) {
	RayTracingStatistics::IncrementRays();
	std::optional<ShapeIntersection> si = m_scene->Intersect(ray);
	if (!si) {
		return Spectrum();
	}
	return Spectrum(glm::abs(si->intr.normal));
}