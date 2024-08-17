#pragma once
#include "Integrator.h"

class TestNormalsIntegrator : public Integrator {
public:
	TestNormalsIntegrator(const glm::ivec2& resolution);

	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;
};