#pragma once
#include "Integrator.h"
#include "RTMath.h"

class RandomWalkIntegrator : public Integrator {
public:
	RandomWalkIntegrator(const glm::ivec2& resolution);

	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	virtual Spectrum IntegrateRandomWalk(Ray ray, Sampler* sampler, uint32_t depth = 0);
};