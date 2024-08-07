#include "TestSamplerIntegrator.h"

TestSamplerIntegrator::TestSamplerIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Vec3 TestSamplerIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Float u = sampler->Get();
	return Vec3(u, u, u);
}