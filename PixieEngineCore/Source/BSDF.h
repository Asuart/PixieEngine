#pragma once
#include "pch.h"
#include "TrowbridgeReitzDistribution.h"
#include "Frame.h"
#include "BxDF.h"
#include "BSDFSample.h"
#include "RTMath.h"
#include "Spectrum.h"

class DiffuseBxDF : public BxDF {
public:
	DiffuseBxDF(Spectrum spectrum);

	Spectrum f(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize();
	BxDFFlags Flags() const override;

protected:
	Spectrum m_spectrum;
};

class DielectricBxDF : public BxDF {
public:
	DielectricBxDF(float refraction, TrowbridgeReitzDistribution mfDistrib);

	BxDFFlags Flags() const override;
	std::optional<BSDFSample> Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Spectrum f(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize();

protected:
	float m_refraction;
	TrowbridgeReitzDistribution m_mfDistrib;
};

class DiffuseTransmissionBxDF : public BxDF {
public:
	DiffuseTransmissionBxDF(Spectrum diffusionSpectrum, Spectrum transmissionSpectrum);

	Spectrum f(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize() override;
	BxDFFlags Flags() const override;

protected:
	Spectrum m_diffusionSpectrum, m_transmissionSpectrum;
};

class BSDF {
	BxDF* bxdf;
	Frame shadingFrame;

public:
	BSDF(Vec3 ns, Vec3 dpdus, BxDF* bxdf);
	~BSDF();

	BxDFFlags Flags() const;
	Vec3 RenderToLocal(Vec3 v) const;
	Vec3 LocalToRender(Vec3 v) const;
	Spectrum f(Vec3 woRender, Vec3 wiRender, TransportMode mode = TransportMode::Radiance) const;
	std::optional<BSDFSample> Sample_f(Vec3 woRender, Float u, Vec2 u2, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Float PDF(Vec3 woRender, Vec3 wiRender, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Spectrum rho(Vec3 woRender, std::span<const Float> uc, std::span<const Vec2> u) const;
	void Regularize();

	operator bool() const;
};
