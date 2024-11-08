#pragma once
#include "RayTracing.h"

class TestNormalsIntegrator : public RayTracer {
public:
	virtual Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) override;
};