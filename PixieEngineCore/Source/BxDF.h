#pragma once
#include "pch.h"
#include "RTMath.h"
#include "BSDFSample.h"
#include "Spectrum.h"

class BxDF {
public:
	virtual BxDFFlags Flags() const = 0;
	virtual Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const = 0;
	virtual BSDFSample SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	virtual Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	Spectrum rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u2) const;
	virtual void Regularize();
};
