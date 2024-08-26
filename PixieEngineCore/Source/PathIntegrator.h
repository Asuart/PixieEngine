#pragma once
#include "pch.h"
#include "Integrator.h"
#include "Random.h"
#include "VisibleSurface.h"
#include "UniformLightSampler.h"

class PathIntegrator : public Integrator {
public:
	bool m_regularize = true;

	PathIntegrator(const glm::ivec2& resolution);
	~PathIntegrator();

	virtual void SetScene(Scene* scene) override;
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	LightSampler* m_lightSampler = nullptr;

	Spectrum SampleLd(const SurfaceInteraction& intr, const BSDF& bsdf, Sampler* sampler);
};