#pragma once
#include "pch.h"
#include "Math/Random.h"
#include "Math/Hash.h"

class Sampler {
public:
	virtual Sampler* Clone() const = 0;
	virtual int32_t SamplesPerPixel() const = 0;
	virtual void StartPixelSample(glm::ivec2 p, int32_t sampleIndex, int32_t dimension = 0) = 0;
	virtual Float Get1D() = 0;
	virtual Vec2 Get2D() = 0;
	virtual Vec2 GetPixel2D() = 0;
};

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

class StratifiedSampler : public Sampler {
public:
    StratifiedSampler(int32_t xPixelSamples, int32_t yPixelSamples, bool jitter, int32_t seed = 0);

    Sampler* Clone() const override;
    int32_t SamplesPerPixel() const override;
    void StartPixelSample(glm::ivec2 p, int32_t index, int32_t dim) override;
    Float Get1D() override;
    Vec2 Get2D() override;
    Vec2 GetPixel2D() override;

protected:
    RNG m_rng;
    int32_t m_xPixelSamples;
    int32_t m_yPixelSamples;
    bool m_jitter;
    int32_t m_seed;
    Vec2 m_pixel = glm::ivec2(0);
    int32_t m_sampleIndex = 0;
    int32_t m_dimension = 0;
};
