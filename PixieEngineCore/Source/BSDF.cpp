#include "pch.h"
#include "BSDF.h"
#include "Material.h"

BSDF::BSDF(const Material& material, const SurfaceInteraction& intr)
	: m_frame(Frame::FromZ(intr.normal)) {
	if (material.m_transparency) {
		Vec2 anisotropicRoughness = Vec2(1.0f, 1.0f);
		bool normalizeAnisotropicRoughness = false;
		Float urough = material.m_roughness * anisotropicRoughness.x;
		Float vrough = material.m_roughness * anisotropicRoughness.y;
		if (normalizeAnisotropicRoughness) {
			urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
			vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
		}
		TrowbridgeReitzDistribution distrib(urough, vrough);
		m_bxdf = new DielectricBxDF(material.m_refraction, distrib);
	}
	else {
		m_bxdf = new DiffuseBxDF(material.m_albedo);
	}
}

BSDF::~BSDF() {
	delete m_bxdf;
}

BSDF::operator bool() const {
	return m_bxdf != nullptr;
}

BxDFFlags BSDF::Flags() const {
	return m_bxdf->Flags();
}

Spectrum BSDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	return m_bxdf->SampleDistribution(wo, wi, mode);
}

std::optional<BSDFSample> BSDF::SampleDirectionAndDistribution(Vec3 wo, Float u, Vec2 u2, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!(Flags() & (int32_t)sampleFlags)) return BSDFSample();
	wo = m_frame.ToLocal(wo);
	std::optional<BSDFSample> sample = m_bxdf->SampleDirectionAndDistribution(wo, u, u2, mode, sampleFlags);
	if (sample) {
		sample->wi = m_frame.FromLocal(sample->wi);
	}
	return sample;
}

Float BSDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	return m_bxdf->PDF(wo, wi, mode, sampleFlags);
}

Spectrum BSDF::rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u) const {
	wo = m_frame.ToLocal(wo);
	return m_bxdf->rho(wo, uc, u);
}

void BSDF::Regularize() {
	m_bxdf->Regularize();
}
