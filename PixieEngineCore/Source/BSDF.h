#pragma once
#include "pch.h"
#include "Frame.h"
#include "MathAndPhysics.h"
#include "Spectrum.h"
#include "TrowbridgeReitzDistribution.h"

struct Material;
struct RayInteraction;


enum class TransportMode {
	Radiance,
	Importance
};

enum class LightFlags : uint32_t {
	DeltaPosition = 1,
	DeltaDirection = 2,
	Area = 4,
	Infinite = 8
};

enum BxDFFlags {
	Unset = 0,
	Reflection = 1 << 0,
	Transmission = 1 << 1,
	Diffuse = 1 << 2,
	Glossy = 1 << 3,
	Specular = 1 << 4,
	DiffuseReflection = Diffuse | Reflection,
	DiffuseTransmission = Diffuse | Transmission,
	GlossyReflection = Glossy | Reflection,
	GlossyTransmission = Glossy | Transmission,
	SpecularReflection = Specular | Reflection,
	SpecularTransmission = Specular | Transmission,
	All = Diffuse | Glossy | Specular | Reflection | Transmission
};

enum class BxDFReflTransFlags {
	Unset = 0,
	Reflection = 1 << 0,
	Transmission = 1 << 1,
	All = Reflection | Transmission
};

bool IsReflective(BxDFFlags f);
bool IsTransmissive(BxDFFlags f);
bool IsDiffuse(BxDFFlags f);
bool IsGlossy(BxDFFlags f);
bool IsSpecular(BxDFFlags f);
bool IsNonSpecular(BxDFFlags f);

struct BSDFSample {
	Spectrum f;
	Vec3 wi;
	Float pdf;
	BxDFFlags flags;
	Float eta;
	bool pdfIsProportional;

	BSDFSample(Spectrum f = Spectrum(), Vec3 wi = Vec3(0, 0, 1), Float pdf = 0.0, BxDFFlags flags = BxDFFlags::All, Float eta = 1, bool pdfIsProportional = false);

	bool IsReflection() const;
	bool IsTransmission() const;
	bool IsDiffuse() const;
	bool IsGlossy() const;
	bool IsSpecular() const;

	operator bool() const;
};

class BxDF {
public:
	virtual BxDFFlags Flags() const = 0;
	virtual Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const = 0;
	virtual std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode = TransportMode::Importance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	virtual Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	Spectrum rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u2) const;
	virtual void Regularize() {};
};

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

class DiffuseTransmissionBxDF : public BxDF {
public:
	DiffuseTransmissionBxDF(Spectrum r, Spectrum t);
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	BxDFFlags Flags() const override;

private:
	Spectrum m_r, m_t;
};

class ConductorBxDF : public BxDF {
public:
	ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib, Spectrum eta, Spectrum k);

	BxDFFlags Flags() const override;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode = TransportMode::Importance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize() override;

protected:
	TrowbridgeReitzDistribution m_mfDistrib;
	Spectrum m_eta;
	Spectrum m_k;
};

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

class BSDF {
public:
	BSDF(const Material& material, const RayInteraction& intr);
	~BSDF();

	BxDFFlags Flags() const;
	Spectrum SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode = TransportMode::Importance) const;
	std::optional<BSDFSample> SampleDirectionAndDistribution(Vec3 wo, Float u, Vec2 u2, TransportMode mode = TransportMode::Importance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Float PDF(Vec3 wo, Vec3 wi, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	Spectrum rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u) const;
	void Regularize();

	operator bool() const;

protected:
	BxDF* m_bxdf = nullptr;
	Frame m_frame = Frame::FromZ(Vec3(0, 0, 1));
};
