#pragma once
#include "pch.h"
#include "Frame.h"
#include "BxDF.h"
#include "BSDFSample.h"
#include "RTMath.h"
#include "Spectrum.h"
#include "DiffuseBxDF.h"
#include "DielectricBxDF.h"

struct Material;
struct SurfaceInteraction;

class BSDF {
public:
	BSDF(const Material& material, const SurfaceInteraction& intr);
	~BSDF();

	BxDFFlags Flags() const;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode = TransportMode::Radiance) const;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float u, Vec2 u2, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Spectrum rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u) const;
	void Regularize();

	operator bool() const;

protected:
	BxDF* m_bxdf = nullptr;
	Frame m_frame = Frame::FromZ(Vec3(0, 0, 1));
};
