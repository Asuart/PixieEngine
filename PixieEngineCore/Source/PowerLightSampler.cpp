#include "pch.h"
#include "PowerLightSampler.h"

PowerLightSampler::PowerLightSampler(const std::vector<Light*>& lights) 
	: m_lights(lights) {}

std::optional<SampledLight> PowerLightSampler::Sample(const LightSampleContext& context, Float u) const {
	return Sample(u);
}

std::optional<SampledLight> PowerLightSampler::Sample(Float u) const {

}

Float PowerLightSampler::PMF(const LightSampleContext& context, const Light* light) const {
	return PMF(light);
}

Float PowerLightSampler::PMF(const Light* light) const {

}

int32_t PowerLightSampler::LightToIndex(Light* light) {
	auto it = m_lightToIndex.find(light);
	if (it != m_lightToIndex.end()) {
		return it->second;
	}
	return -1;
}
