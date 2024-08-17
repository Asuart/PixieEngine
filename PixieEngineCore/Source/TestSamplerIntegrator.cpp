#include "TestSamplerIntegrator.h"

TestSamplerIntegrator::TestSamplerIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum TestSamplerIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Float u = sampler->Get();
	return Spectrum(u, u, u);
}