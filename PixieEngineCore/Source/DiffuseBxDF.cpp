#include "pch.h"
#include "DiffuseBxDF.h"

DiffuseBxDF::DiffuseBxDF(Spectrum spectrum)
	: m_spectrum(spectrum) {}

Spectrum DiffuseBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi)) {
		return Spectrum();
	}
	return m_spectrum * InvPi;
}

std::optional<BSDFSample> DiffuseBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection)) {
		return {};
	}
	Vec3 wi = SampleCosineHemisphere(u);
	if (wo.z < 0) wi.z *= -1;
	Float pdf = CosineHemispherePDF(AbsCosTheta(wi));
	return BSDFSample(m_spectrum * InvPi, wi, pdf, BxDFFlags::DiffuseReflection);
}

Float DiffuseBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection) || !SameHemisphere(wo, wi)) {
		return 0;
	}
	return CosineHemispherePDF(AbsCosTheta(wi));
}

BxDFFlags DiffuseBxDF::Flags() const {
	return m_spectrum ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset;
}
