#pragma once 
#include "pch.h"
#include "BxDF.h"

class DiffuseBxDF : public BxDF {
public:
	DiffuseBxDF(Spectrum spectrum);

	BxDFFlags Flags() const override;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;

protected:
	Spectrum m_spectrum;
};
