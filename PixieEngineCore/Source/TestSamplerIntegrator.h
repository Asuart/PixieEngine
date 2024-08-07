#pragma once
#include "Integrator.h"

class TestSamplerIntegrator : public Integrator {
public:
	TestSamplerIntegrator(const glm::ivec2& resolution);

	virtual Vec3 Integrate(Ray ray, Sampler* sampler) override;
};