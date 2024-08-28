#include "pch.h"
#include "TestSamplerIntegrator.h"

TestSamplerIntegrator::TestSamplerIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum TestSamplerIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Float u = sampler->Get1D();
	return Spectrum(u, u, u);
}