#pragma once
#include "Integrator.h"
#include "RTMath.h"

class RandomWalkIntegrator : public Integrator {
public:
	RandomWalkIntegrator(const glm::ivec2& resolution);

	virtual Vec3 Integrate(Ray ray, Sampler* sampler) override;

protected:
	virtual Vec3 IntegrateRandomWalk(Ray ray, Sampler* sampler, uint32_t depth = 0);
};