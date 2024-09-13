#include "pch.h"
#include "ConductorBxDF.h"

ConductorBxDF::ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib, Spectrum eta, Spectrum k)
	: m_mfDistrib(mfDistrib), m_eta(eta), m_k(k) {}

BxDFFlags ConductorBxDF::Flags() const {
	return m_mfDistrib.EffectivelySmooth() ? BxDFFlags::SpecularReflection : BxDFFlags::GlossyReflection;
}

Spectrum ConductorBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi) || m_mfDistrib.EffectivelySmooth()) {
		return Spectrum();
	}
	Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
	if (cosTheta_i == 0 || cosTheta_o == 0) {
		return Spectrum();
	}
	Vec3 wm = wi + wo;
	if (length2(wm) == 0) {
		return Spectrum();
	}
	wm = glm::normalize(wm);

	Spectrum F = FrComplex(AbsDot(wo, wm), m_eta, m_k);

	return m_mfDistrib.D(wm) * F * m_mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
}

std::optional<BSDFSample> ConductorBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection)) {
		return {};
	}
	if (m_mfDistrib.EffectivelySmooth()) {
		Vec3 wi(-wo.x, -wo.y, wo.z);
		Spectrum f = FrComplex(AbsCosTheta(wi), m_eta, m_k) / AbsCosTheta(wi);
		return BSDFSample(f, wi, 1, BxDFFlags::SpecularReflection);
	}

	if (wo.z == 0) {
		return {};
	}
	Vec3 wm = m_mfDistrib.Sample_wm(wo, u);
	Vec3 wi = Reflect(wo, wm);
	if (!SameHemisphere(wo, wi)) {
		return {};
	}

	Float pdf = m_mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));

	Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
	if (cosTheta_i == 0 || cosTheta_o == 0) {
		return {};
	}
	Spectrum F = FrComplex(AbsDot(wo, wm), m_eta, m_k);

	Spectrum f = m_mfDistrib.D(wm) * F * m_mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
	return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
}

Float ConductorBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection) || !SameHemisphere(wo, wi) || m_mfDistrib.EffectivelySmooth()) {
		return 0;
	}
	Vec3 wm = wo + wi;
	if (length2(wm) == 0) {
		return 0;
	}
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));
	return m_mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));
}

void ConductorBxDF::Regularize() {
	m_mfDistrib.Regularize();
}
