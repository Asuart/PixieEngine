#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Integrator.h"
#include "Random.h"

class PathIntegrator : public Integrator {
public:
	PathIntegrator(const glm::ivec2& resolution);

	virtual void SetScene(RTScene* scene) override;
	virtual Vec3 Integrate(Ray ray) override;

protected:
	bool m_regularize = true;
	UniformLightSampler m_lightSampler;
};