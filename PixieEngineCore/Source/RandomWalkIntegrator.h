#pragma once
#include "Integrator.h"
#include "RTMath.h"

class RandomWalkIntegrator : public Integrator {
public:
	RandomWalkIntegrator(const glm::ivec2& resolution);

	virtual Vec3 Integrate(Ray ray) override;

protected:
	virtual Vec3 IntegrateRandomWalk(Ray ray, uint32_t depth = 0);
};