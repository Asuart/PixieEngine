#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Integrator.h"
#include "Random.h"

class SimplePathIntegrator : public Integrator {
public:
	bool m_sampleLights = true;
	bool m_sampleBSDF = true;

	SimplePathIntegrator(const glm::ivec2& resolution);

	virtual void SetScene(RTScene* scene) override;
	virtual Vec3 Integrate(Ray ray, Sampler* sampler) override;

protected:
	UniformLightSampler m_lightSampler;
};