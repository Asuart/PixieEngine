#include "pch.h"
#include "StratifiedSampler.h"

StratifiedSampler::StratifiedSampler(int32_t xPixelSamples, int32_t yPixelSamples, bool jitter, int32_t seed)
    : m_xPixelSamples(xPixelSamples), m_yPixelSamples(yPixelSamples), m_seed(seed), m_jitter(jitter) {}

Sampler* StratifiedSampler::Clone() const {
    return new StratifiedSampler(m_xPixelSamples, m_yPixelSamples, m_jitter, m_seed);
}

int32_t StratifiedSampler::SamplesPerPixel() const { 
    return m_xPixelSamples * m_yPixelSamples; 
}

void StratifiedSampler::StartPixelSample(glm::ivec2 p, int32_t index, int32_t dim) {
    m_pixel = p;
    m_sampleIndex = index;
    m_dimension = dim;
    m_rng.SetSequence(Hash(p, m_seed));
    m_rng.Advance(m_sampleIndex * 65536ull + m_dimension);
}

Float StratifiedSampler::Get1D() {
    uint64_t hash = Hash(m_pixel, m_dimension, m_seed);
    int32_t stratum = PermutationElement(m_sampleIndex, SamplesPerPixel(), hash);
    m_dimension++;
    Float delta = m_jitter ? m_rng.Uniform<Float>() : 0.5f;
    return (stratum + delta) / SamplesPerPixel();
}

Vec2 StratifiedSampler::Get2D() {
    uint64_t hash = Hash(m_pixel, m_dimension, m_seed);
    int32_t stratum = PermutationElement(m_sampleIndex, SamplesPerPixel(), hash);
    m_dimension += 2;
    int32_t x = stratum % m_xPixelSamples, y = stratum / m_xPixelSamples;
    Float dx = m_jitter ? m_rng.Uniform<Float>() : 0.5f;
    Float dy = m_jitter ? m_rng.Uniform<Float>() : 0.5f;
    return { (x + dx) / m_xPixelSamples, (y + dy) / m_yPixelSamples };
}

Vec2 StratifiedSampler::GetPixel2D() { 
    return Get2D();
}
