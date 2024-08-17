#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Integrator.h"
#include "Random.h"

class PathIntegrator : public Integrator {
public:
	bool m_regularize = true;

	PathIntegrator(const glm::ivec2& resolution);

	virtual void SetScene(Scene* scene) override;
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) override;

protected:
	UniformLightSampler m_lightSampler;

	//Vec3 SampleLd(const SurfaceInteraction& intr, const BSDF* bsdf) const;
};