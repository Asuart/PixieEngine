#pragma once
#include "Integrator.h"

class NormalsTracer : public Integrator {
public:
	NormalsTracer(const glm::ivec2& resolution);

	virtual Vec3 Integrate(Ray ray) override;
};