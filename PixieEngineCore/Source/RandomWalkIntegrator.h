#pragma once
#include "RayTracing.h"
#include "RTMath.h"

class RandomWalkIntegrator : public RayTracer {
public:
	virtual Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) override;

protected:
	virtual Spectrum IntegrateRandomWalk(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler, uint32_t depth = 0);
};
