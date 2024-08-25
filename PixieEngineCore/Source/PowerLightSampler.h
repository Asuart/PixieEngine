#pragma once
#include "pch.h"
#include "LightSampler.h"
#include "AliasTable.h"

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
