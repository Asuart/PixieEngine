#include "pch.h"
#include "BSDF.h"
#include "Material.h"

BSDF::BSDF(const Material& material, const SurfaceInteraction& intr)
	: m_frame(Frame::FromZ(intr.normal)) {
	AddBxDF(new DiffuseBxDF(material.m_albedo), 1.0f - material.m_transparency);
	if (material.m_transparency) {
		const glm::fvec2 anisotropicRoughness = glm::fvec2(1.0f, 1.0f);
		const bool normalizeAnisotropicRoughness = false;
		float urough = material.m_roughness * anisotropicRoughness.x;
		float vrough = material.m_roughness * anisotropicRoughness.y;
		if (normalizeAnisotropicRoughness) {
			urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
			vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
		}
		TrowbridgeReitzDistribution distrib(urough, vrough);
		AddBxDF(new DielectricBxDF(material.m_refraction, distrib), material.m_transparency);
	}
}

BSDF::~BSDF() {
	for (size_t i = 0; i < m_bxdfs.size(); i++) {
		delete m_bxdfs[i];
	}
}

BSDF::operator bool() const {
	return m_bxdfs.size() > 0;
}

BxDFFlags BSDF::Flags() const {
	return m_flags;
}

Spectrum BSDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	BxDF* bxdf = GetRandomBxDF();
	return bxdf->SampleDistribution(wo, wi, mode);
}

BSDFSample BSDF::SampleDirectionAndDistribution(Vec3 wo, Float u, Vec2 u2, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!(m_flags & (int32_t)sampleFlags)) return BSDFSample();
	wo = m_frame.ToLocal(wo);
	BxDF* bxdf = GetRandomBxDF();
	BSDFSample sample = bxdf->SampleDirectionAndDistribution(wo, u, u2, mode, sampleFlags);
	sample.wi = m_frame.FromLocal(sample.wi);
	return sample;
}

Float BSDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	BxDF* bxdf = GetRandomBxDF();
	return bxdf->PDF(wo, wi, mode, sampleFlags);
}

Spectrum BSDF::rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u) const {
	wo = m_frame.ToLocal(wo);
	BxDF* bxdf = GetRandomBxDF();
	return bxdf->rho(wo, uc, u);
}

void BSDF::Regularize() {
	for (size_t i = 0; i < m_bxdfs.size(); i++) {
		m_bxdfs[i]->Regularize();
	}
}

void BSDF::AddBxDF(BxDF* bxdf, Float weight) {
	m_bxdfs.push_back(bxdf);
	m_bxdfWeights.push_back(weight);
	m_bxdfWeightsSum += weight;
	m_flags = (BxDFFlags)((uint32_t)m_flags | (uint32_t)bxdf->Flags());
}

BxDF* BSDF::GetRandomBxDF() const {
	Float point = RandomFloat() * m_bxdfWeightsSum;
	int32_t index = 0;
	while (point > m_bxdfWeights[index] && index < m_bxdfs.size() - 1) {
		point -= m_bxdfWeights[index];
		index++;
	}
	return m_bxdfs[index];
}