#pragma once 
#include "pch.h"
#include "Buffer2D.h"
#include "Bounds.h"
#include "PiecewiseConstant.h"

struct FilterSample {
	Vec2 p = Vec2(0); // point position
	Float weight = 1.0f;

	FilterSample() = default;
	FilterSample(Vec2 p, Float weight);
};

class SampleFilter;

class FilterSampler {
public:
	FilterSampler(SampleFilter* filter);

	FilterSample Sample(Vec2 u) const;

protected:
	Bounds2f m_domain;
	Buffer2D<Float> m_f;
	PiecewiseConstant2D m_distrib;
};

class SampleFilter {
public:
	virtual Float Evaluate(Vec2 p) const = 0;
	virtual Float Integral() const = 0;
	virtual FilterSample Sample(Vec2 u) const = 0;
	virtual Vec2 Radius() const = 0;
};

class GaussianFilter : public SampleFilter {
public:
	GaussianFilter(Vec2 radius, Float sigma = 0.5f);

	Float Evaluate(Vec2 p) const override;
	Float Integral() const override;
	FilterSample Sample(Vec2 u) const override;
	Vec2 Radius() const override;

protected:
	Vec2 m_radius;
	Float m_sigma;
	Float m_expX;
	Float m_expY;
	FilterSampler m_sampler;
};