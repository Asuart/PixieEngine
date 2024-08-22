#pragma once
#include "pch.h"
#include "Integrator.h"
#include "Random.h"
#include "VisibleSurface.h"

class PathIntegrator : public Integrator {
public:
	bool m_regularize = true;

	PathIntegrator(const glm::ivec2& resolution);

	virtual void SetScene(Scene* scene) override;
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	UniformLightSampler m_lightSampler;

	Spectrum SampleLd(uint32_t x, uint32_t y, const SurfaceInteraction& intr, const BSDF& bsdf, Sampler* sampler);
};