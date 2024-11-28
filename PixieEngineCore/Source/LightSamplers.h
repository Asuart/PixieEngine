#pragma once
#include "pch.h"
#include "Lights.h"
#include "AliasTable.h"

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

class PowerLightSampler : public LightSampler {
public:
	PowerLightSampler(const std::vector<Light*>& lights);

	std::optional<SampledLight> Sample(const LightSampleContext& context, Float u) const override;
	std::optional<SampledLight> Sample(Float u) const override;
	Float PMF(const LightSampleContext& context, const Light* light) const override;
	Float PMF(const Light* light) const override;

protected:
	const std::vector<Light*>& m_lights;
	std::map<const Light*, int32_t> m_lightToIndex;
	AliasTable m_aliasTable;

	int32_t LightToIndex(const Light* light) const;
};
