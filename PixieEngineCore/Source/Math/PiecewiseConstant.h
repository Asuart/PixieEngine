#pragma once
#include "pch.h"
#include "Math.h"
#include "Buffer2D.h"
#include "Bounds.h"

class PiecewiseConstant1D {
public:
	PiecewiseConstant1D() = default;
	PiecewiseConstant1D(std::vector<Float> f);
	PiecewiseConstant1D(std::vector<Float> f, Float min, Float max);

	size_t BytesUsed() const;
	Float Integral() const;
	size_t size() const;
	Float Sample(Float u, Float* pdf = nullptr, int32_t* offset = nullptr) const;
	std::optional<Float> Invert(Float x) const;

protected:
	std::vector<Float> m_func;
	std::vector<Float> m_cdf;
	Float m_min = 0.0f;
	Float m_max = 1.0f;
	Float m_funcInt = 0;

	friend class PiecewiseConstant2D;
};

class PiecewiseConstant2D {
public:
	PiecewiseConstant2D() = default;
	PiecewiseConstant2D(const Buffer2D<Float>& data);
	PiecewiseConstant2D(const Buffer2D<Float>& data, Bounds2f domain);

	size_t BytesUsed() const;
	Bounds2f Domain() const;
	glm::ivec2 GetResolution() const;
	Float Integral() const;
	Vec2 Sample(Vec2 u, Float* pdf = nullptr, glm::ivec2* offset = nullptr) const;
	Float PDF(Vec2 pr) const;
	std::optional<Vec2> Invert(Vec2 p) const;

protected:
	Bounds2f m_domain;
	std::vector<PiecewiseConstant1D> m_pConditionalV;
	PiecewiseConstant1D m_pMarginal;
};
