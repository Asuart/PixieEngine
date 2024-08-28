#include "pch.h"
#include "IndependentSampler.h"

IndependentSampler::IndependentSampler(int32_t samplesPerPixel, int32_t seed)
	: m_samplesPerPixel(samplesPerPixel), m_seed(seed) {}

int32_t IndependentSampler::SamplesPerPixel() const {
	return m_samplesPerPixel;
}

void IndependentSampler::StartPixelSample(Vec2 p, int32_t sampleIndex, int32_t dimension) {
	m_rng.SetSequence(Hash(p, m_seed));
	m_rng.Advance(sampleIndex * 65536ull + dimension);
}

Float IndependentSampler::Get1D() {
	return m_rng.Uniform<Float>();
}

Vec2 IndependentSampler::Get2D() {
	return Vec2(m_rng.Uniform<Float>(), m_rng.Uniform<Float>());
}

Vec2 IndependentSampler::GetPixel2D() {
	return Get2D();
}
