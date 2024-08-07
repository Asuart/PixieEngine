#include "TestNormalsIntegrator.h"

TestNormalsIntegrator::TestNormalsIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Vec3 TestNormalsIntegrator::Integrate(Ray ray, Sampler* sampler) {
	SurfaceInteraction intr;
	m_scene->Intersect(ray, intr, m_stats);
	return glm::abs(intr.normal);
}