#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTMath.h"
#include "BxDFTools.h"
#include "BSDFSample.h"
#include "Spectrum.h"

class BxDF {
public:
	virtual BxDFFlags Flags() const = 0;
	virtual Spectrum f(Vec3 wo, Vec3 wi, TransportMode mode) const = 0;
	virtual std::optional<BSDFSample> Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	virtual Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	Spectrum rho(Vec3 wo, std::span<const Float> uc, std::span<const Vec2> u2) const;
	virtual void Regularize() = 0;
};
