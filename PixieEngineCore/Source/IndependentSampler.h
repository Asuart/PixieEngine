#pragma once
#include "Sampler.h"

class IndependentSampler : public Sampler {
public:
	IndependentSampler(int32_t samplesPerPixel, int32_t seed = 0);

	Sampler* Clone() const override;
	int32_t SamplesPerPixel() const override;
	void StartPixelSample(glm::ivec2 p, int32_t sampleIndex, int32_t dimension = 0) override;
	Float Get1D() override;
	Vec2 Get2D() override;
	Vec2 GetPixel2D() override;

protected:
	int32_t m_samplesPerPixel, m_seed;
	RNG m_rng;
};
