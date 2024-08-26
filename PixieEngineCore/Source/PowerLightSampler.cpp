#include "pch.h"
#include "PowerLightSampler.h"

PowerLightSampler::PowerLightSampler(const std::vector<Light*>& lights) 
	: m_lights(lights) {
    if (lights.empty()) {
        return;
    }
    for (size_t i = 0; i < lights.size(); ++i) {
        m_lightToIndex[lights[i]] = (int32_t)i;
    }
    std::vector<Float> lightPower;
    for (const auto& light : lights) {
        lightPower.push_back(light->Phi().Average());
    }
    if (std::accumulate(lightPower.begin(), lightPower.end(), 0.0f) == 0.0f) {
        std::fill(lightPower.begin(), lightPower.end(), 1.0f);
    }
    m_aliasTable = AliasTable(lightPower);
}

std::optional<SampledLight> PowerLightSampler::Sample(const LightSampleContext& context, Float u) const {
	return Sample(u);
}

std::optional<SampledLight> PowerLightSampler::Sample(Float u) const {
    if (!m_aliasTable.size()) {
        return {};
    }
    Float pmf;
    int32_t lightIndex = m_aliasTable.Sample(u, &pmf);
    return SampledLight{ m_lights[lightIndex], pmf };
}

Float PowerLightSampler::PMF(const LightSampleContext& context, const Light* light) const {
	return PMF(light);
}

Float PowerLightSampler::PMF(const Light* light) const {
    if (!m_aliasTable.size()) {
        return 0;
    }
    return m_aliasTable.PMF(LightToIndex(light));
}

int32_t PowerLightSampler::LightToIndex(const Light* light) const {
	auto it = m_lightToIndex.find(light);
	if (it != m_lightToIndex.end()) {
		return it->second;
	}
	return -1;
}
