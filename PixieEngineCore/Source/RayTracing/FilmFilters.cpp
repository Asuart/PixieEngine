#include "pch.h"
#include "FilmFilters.h"

/*
	FIlm Filter Sample
*/

FilmFilterSample::FilmFilterSample(Vec2 p, Float weight) :
	p(p), weight(weight) {}

/*
	Film Filter Sampler
*/

FilmFilterSampler::FilmFilterSampler(FilmFilter* filter) :
	m_domain(Vec2(-filter->Radius()), Vec2(filter->Radius())),
	m_f({ int32_t(32 * filter->Radius().x), int32_t(32 * filter->Radius().y) }) {
	for (int32_t y = 0; y < m_f.GetHeight(); y++) {
		for (int32_t x = 0; x < m_f.GetWidth(); x++) {
			Vec2 p = m_domain.Lerp(Vec2((x + 0.5f) / m_f.GetWidth(), (y + 0.5f) / m_f.GetHeight()));
			m_f.SetValue(x, y, filter->Evaluate(p));
		}
	}
	m_distrib = PiecewiseConstant2D(m_f, m_domain);
}

FilmFilterSample FilmFilterSampler::Sample(Vec2 u) const {
	Float pdf;
	glm::ivec2 pi;
	Vec2 p = m_distrib.Sample(u, &pdf, &pi);
	return FilmFilterSample(p, m_f.GetValue(pi.x, pi.y) / pdf);
}

/*
	Gaussian Filter
*/

GaussianFilter::GaussianFilter(Vec2 radius, Float sigma) :
	m_radius(radius), m_sigma(sigma),
	m_expX(Gaussian(radius.x, 0, sigma)),
	m_expY(Gaussian(radius.y, 0, sigma)),
	m_sampler(this) {}

Float GaussianFilter::Evaluate(Vec2 p) const {
	return (std::max<Float>(0, Gaussian(p.x, 0, m_sigma) - m_expX) * std::max<Float>(0, Gaussian(p.y, 0, m_sigma) - m_expY));
}

Float GaussianFilter::Integral() const {
	return ((GaussianIntegral(-m_radius.x, m_radius.x, 0, m_sigma) - 2 * m_radius.x * m_expX) * (GaussianIntegral(-m_radius.y, m_radius.y, 0, m_sigma) - 2 * m_radius.y * m_expY));
}

FilmFilterSample GaussianFilter::Sample(Vec2 u) const {
	return m_sampler.Sample(u);
}

Vec2 GaussianFilter::Radius() const {
	return m_radius;
}
