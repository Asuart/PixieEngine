#pragma once
#include "pch.h"
#include "RayTracing.h"
#include "Random.h"
#include "UniformLightSampler.h"

class SimplePathIntegrator : public RayTracer {
public:
	bool m_sampleLights = true;
	bool m_sampleBSDF = true;

	SimplePathIntegrator();
	~SimplePathIntegrator();

	void PreprocessSceneSnapshot(SceneSnapshot* sceneSnapshot) override;
	Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) override;

protected:
	UniformLightSampler* m_lightSampler;
};
