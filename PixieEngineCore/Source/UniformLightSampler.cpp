#include "pch.h"
#include "UniformLightSampler.h"

UniformLightSampler::UniformLightSampler(std::vector<Light*>* lights) 
	: m_lights(lights) {}

std::optional<SampledLight> UniformLightSampler::Sample(const LightSampleContext& context, Float u) const {
	return Sample(u);
}

std::optional<SampledLight> UniformLightSampler::Sample(Float u) const {
	if (!m_lights || m_lights->empty()) {
		return {};
	}
	int32_t lightIndex = std::min<int32_t>((int32_t)(u * m_lights->size()), (int32_t)m_lights->size() - 1);
	return SampledLight((*m_lights)[lightIndex], 1.0f / m_lights->size());
}

Float UniformLightSampler::PMF(const LightSampleContext& context, const Light* light) const {
	return PMF(light);
}

Float UniformLightSampler::PMF(const Light* light) const {
	if (!m_lights || m_lights->empty()) {
		return 0.0f;
	}
	return 1.0f / m_lights->size();
}
