#pragma once
#include "pch.h"
#include "LightSampler.h"

class UniformLightSampler : public LightSampler {
public:
	UniformLightSampler() = default;
	UniformLightSampler(std::vector<Light*>* lights);

	std::optional<SampledLight> Sample(const LightSampleContext& context, Float u) const override;
	std::optional<SampledLight> Sample(Float u) const override;
	Float PMF(const LightSampleContext& context, const Light* light) const override;
	Float PMF(const Light* light) const override;

protected:
	std::vector<Light*>* m_lights = nullptr;
};
