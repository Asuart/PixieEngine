#pragma once
#include "pch.h"
#include "Integrator.h"
#include "Random.h"
#include "UniformLightSampler.h"

class SimplePathIntegrator : public Integrator {
public:
	bool m_sampleLights = true;
	bool m_sampleBSDF = true;

	SimplePathIntegrator(const glm::ivec2& resolution);
	~SimplePathIntegrator();

	virtual void SetScene(Scene* scene) override;
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	LightSampler* m_lightSampler;
};