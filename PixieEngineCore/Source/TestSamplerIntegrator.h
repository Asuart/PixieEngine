#pragma once
#include "RayTracing.h"

class TestSamplerIntegrator : public RayTracer {
public:
	virtual Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) override;
};
