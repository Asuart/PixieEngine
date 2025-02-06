#include "pch.h"
#include "LightSamplers.h"

/*
    Sampled Light
*/

SampledLight::SampledLight(Light* light, Float p) :
    light(light), p(p) {}

/*
    Uniform Light Sampler
*/

UniformLightSampler::UniformLightSampler(std::vector<Light*>* lights) :
    m_lights(lights) {}

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

/*
    Power Light Sampler
*/

PowerLightSampler::PowerLightSampler(const std::vector<Light*>& lights) :
    m_lights(lights) {
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
