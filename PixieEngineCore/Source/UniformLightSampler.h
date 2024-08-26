#pragma once
#include "pch.h"
#include "LightSampler.h"

class UniformLightSampler : public LightSampler {
public:
	UniformLightSampler() = default;
	UniformLightSampler(const std::vector<Light*>& lights);

	std::optional<SampledLight> Sample(const LightSampleContext& context, Float u) const override;
	std::optional<SampledLight> Sample(Float u) const override;
	Float PMF(const LightSampleContext& context, const Light* light) const override;
	Float PMF(const Light* light) const override;

protected:
	const std::vector<Light*>& m_lights = std::vector<Light*>(0);
};
