#pragma once
#include "pch.h"
#include "RayTracing.h"
#include "Random.h"
#include "UniformLightSampler.h"

class PathIntegrator : public RayTracer {
public:
	bool m_regularize = true;

	PathIntegrator();
	~PathIntegrator();

	void PreprocessSceneSnapshot(SceneSnapshot* scene) override;
	Spectrum SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) override;

protected:
	LightSampler* m_lightSampler = nullptr;

	Spectrum SampleLd(SceneSnapshot* sceneSnapshot, const SurfaceInteraction& intr, const BSDF& bsdf, Sampler* sampler);
};
