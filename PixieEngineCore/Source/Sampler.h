#pragma once
#include "pch.h"
#include "Random.h"
#include "Hash.h"

class Sampler {
public:
	virtual int32_t SamplesPerPixel() const = 0;
	virtual void StartPixelSample(Vec2 p, int32_t sampleIndex, int32_t dimension = 0) = 0;
	virtual Float Get1D() = 0;
	virtual Vec2 Get2D() = 0;
	virtual Vec2 GetPixel2D() = 0;
};
