#include "pch.h"
#include "PiecewiseConstant.h"

PiecewiseConstant1D::PiecewiseConstant1D(std::vector<Float> f)
	: PiecewiseConstant1D(f, 0.0f, 1.0f) {}

PiecewiseConstant1D::PiecewiseConstant1D(std::vector<Float> f, Float min, Float max)
	: m_func(f.begin(), f.end()), m_cdf(f.size() + 1), m_min(min), m_max(max) {
	for (Float& f : m_func) {
		f = std::abs(f);
	}

	m_cdf[0] = 0;
	size_t n = f.size();
	for (size_t i = 1; i < n + 1; ++i) {
		m_cdf[i] = m_cdf[i - 1] + m_func[i - 1] * (max - min) / n;
	}

	m_funcInt = m_cdf[n];
	if (m_funcInt == 0) {
		for (size_t i = 1; i < n + 1; ++i) {
			m_cdf[i] = Float(i) / Float(n);
		}
	}
	else {
		for (size_t i = 1; i < n + 1; ++i) {
			m_cdf[i] /= m_funcInt;
		}
	}
}

size_t PiecewiseConstant1D::BytesUsed() const {
	return (m_func.capacity() + m_cdf.capacity()) * sizeof(Float);
}

Float PiecewiseConstant1D::Integral() const {
	return m_funcInt;
}

size_t PiecewiseConstant1D::size() const {
	return m_func.size();
}

Float PiecewiseConstant1D::Sample(Float u, Float* pdf, int32_t* offset) const {
	int32_t o = (int32_t)FindInterval((int32_t)m_cdf.size(), [&](int32_t index) { return m_cdf[index] <= u; });
	if (offset) {
		*offset = o;
	}

	Float du = u - m_cdf[o];
	if (m_cdf[o + 1] - m_cdf[o] > 0) {
		du /= m_cdf[o + 1] - m_cdf[o];
	}

	if (pdf) {
		*pdf = (m_funcInt > 0) ? m_func[o] / m_funcInt : 0;
	}

	return Lerp((o + du) / size(), m_min, m_max);
}

std::optional<Float> PiecewiseConstant1D::Invert(Float x) const {
	if (x < m_min || x > m_max) {
		return {};
	}
	Float c = (x - m_min) / (m_max - m_min) * m_func.size();
	int32_t offset = Clamp(int32_t(c), 0, m_func.size() - 1);

	Float delta = c - offset;
	return Lerp(delta, m_cdf[offset], m_cdf[offset + 1]);
}

PiecewiseConstant2D::PiecewiseConstant2D(const Buffer2D<Float>& data)
	: PiecewiseConstant2D(data, Bounds2f(Vec2(0, 0), Vec2(1, 1))) {}

PiecewiseConstant2D::PiecewiseConstant2D(const Buffer2D<Float>& data, Bounds2f domain)
	: m_domain(domain) {
	int32_t nu = data.GetWidth();
	int32_t nv = data.GetHeight();
	m_pConditionalV.reserve(nv);
	for (int32_t v = 0; v < nv; v++) {
		m_pConditionalV.emplace_back(data.CopyRow(v), domain.min[0], domain.max[0]);
	}

	std::vector<Float> marginalFunc;
	marginalFunc.reserve(nv);
	for (int32_t v = 0; v < nv; v++) {
		marginalFunc.push_back(m_pConditionalV[v].Integral());
	}
	m_pMarginal = PiecewiseConstant1D(marginalFunc, domain.min[1], domain.max[1]);
}

size_t PiecewiseConstant2D::BytesUsed() const {
	return m_pConditionalV.size() * (m_pConditionalV[0].BytesUsed() + sizeof(m_pConditionalV[0])) + m_pMarginal.BytesUsed();
}

Bounds2f PiecewiseConstant2D::Domain() const {
	return m_domain;
}

glm::ivec2 PiecewiseConstant2D::GetResolution() const {
	return { int32_t(m_pConditionalV[0].size()), int32_t(m_pMarginal.size()) };
}

Float PiecewiseConstant2D::Integral() const { 
	return m_pMarginal.Integral();
}

Vec2 PiecewiseConstant2D::Sample(Vec2 u, Float* pdf, glm::ivec2* offset) const {
	Float pdfs[2];
	glm::ivec2 uv;
	Float d1 = m_pMarginal.Sample(u[1], &pdfs[1], &uv[1]);
	Float d0 = m_pConditionalV[uv[1]].Sample(u[0], &pdfs[0], &uv[0]);
	if (pdf) {
		*pdf = pdfs[0] * pdfs[1];
	}
	if (offset) {
		*offset = uv;
	}
	return Vec2(d0, d1);
}

Float PiecewiseConstant2D::PDF(Vec2 pr) const {
	Vec2 p = Vec2(m_domain.Offset(pr));
	int32_t iu = Clamp(int32_t(p[0] * m_pConditionalV[0].size()), 0, m_pConditionalV[0].size() - 1);
	int32_t iv = Clamp(int32_t(p[1] * m_pMarginal.size()), 0, m_pMarginal.size() - 1);
	return m_pConditionalV[iv].m_func[iu] / m_pMarginal.Integral();
}

std::optional<Vec2> PiecewiseConstant2D::Invert(Vec2 p) const {
	std::optional<Float> mInv = m_pMarginal.Invert(p[1]);
	if (!mInv) {
		return {};
	}
	Float p1o = (p[1] - m_domain.min[1]) / (m_domain.max[1] - m_domain.min[1]);
	if (p1o < 0 || p1o > 1) {
		return {};
	}
	int32_t offset = Clamp(p1o * m_pConditionalV.size(), 0, m_pConditionalV.size() - 1);
	std::optional<Float> cInv = m_pConditionalV[offset].Invert(p[0]);
	if (!cInv) {
		return {};
	}
	return Vec2(*cInv, *mInv);
}
