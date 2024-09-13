#pragma once
#include "BxDF.h"
#include "TrowbridgeReitzDistribution.h"

class ConductorBxDF : public BxDF {
public:
	ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib, Spectrum eta, Spectrum k);

	BxDFFlags Flags() const override;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize() override;

protected:
	TrowbridgeReitzDistribution m_mfDistrib;
	Spectrum m_eta;
	Spectrum m_k;
};
