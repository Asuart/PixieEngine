#include "TestNormalsIntegrator.h"

TestNormalsIntegrator::TestNormalsIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum TestNormalsIntegrator::Integrate(Ray ray, Sampler* sampler) {
	SurfaceInteraction intr;
	m_scene->Intersect(ray, intr, m_stats);
	return Spectrum(glm::abs(intr.normal));
}