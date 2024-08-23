#pragma once
#include "pch.h"
#include "Light.h"

struct SampledLight {
	Light* light = nullptr; // Sampled light source.
	Float p = 0.0f; // Probability to sample light source.

	SampledLight() = default;
	SampledLight(Light* light, Float p);
};

class LightSampler {
public:
	virtual std::optional<SampledLight> Sample(const LightSampleContext& context, Float u) const = 0;
	virtual std::optional<SampledLight> Sample(Float u) const = 0;
	virtual Float PMF(const LightSampleContext& context, const Light* light) const = 0;
	virtual Float PMF(const Light* light) const = 0;
};
