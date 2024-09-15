#pragma once
#include "BxDF.h"
#include "TrowbridgeReitzDistribution.h"

class DielectricBxDF : public BxDF {
public:
	DielectricBxDF(Float refraction, TrowbridgeReitzDistribution mfDistrib);

	BxDFFlags Flags() const override;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize() override;

protected:
	TrowbridgeReitzDistribution m_mfDistrib;
	Float m_refraction;
};
