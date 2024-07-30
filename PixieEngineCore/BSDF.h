#ifndef PIXIE_ENGINE_BSDF
#define PIXIE_ENGINE_BSDF

#include "pch.h"
#include "TrowbridgeReitzDistribution.h"
#include "Frame.h"
#include "BxDF.h"
#include "BSDFSample.h"

class DiffuseBxDF : public BxDF {
	glm::vec3 R;

public:
	DiffuseBxDF(glm::vec3 R);

	glm::vec3 f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	float PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize();
	BxDFFlags Flags() const override;
};

class DielectricBxDF : public BxDF {
	float eta;
	TrowbridgeReitzDistribution mfDistrib;

public:
	DielectricBxDF(float eta, TrowbridgeReitzDistribution mfDistrib);

	BxDFFlags Flags() const override;
	std::optional<BSDFSample> Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	glm::vec3 f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const override;
	float PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize();
};

class DiffuseTransmissionBxDF : public BxDF {
	glm::vec3 R, T;

public:
	DiffuseTransmissionBxDF(glm::vec3 R, glm::vec3 T);

	glm::vec3 f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const override;
	std::optional<BSDFSample> Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	float PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const override;
	void Regularize() override;
	BxDFFlags Flags() const override;
};

class BSDF {
	BxDF* bxdf;
	Frame shadingFrame;

public:
	BSDF(glm::vec3 ns, glm::vec3 dpdus, BxDF* bxdf);
	~BSDF();

	BxDFFlags Flags() const;
	glm::vec3 RenderToLocal(glm::vec3 v) const;
	glm::vec3 LocalToRender(glm::vec3 v) const;
	glm::vec3 f(glm::vec3 woRender, glm::vec3 wiRender, TransportMode mode = TransportMode::Radiance) const;
	std::optional<BSDFSample> Sample_f(glm::vec3 woRender, float u, glm::vec2 u2, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	float PDF(glm::vec3 woRender, glm::vec3 wiRender, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const;
	glm::vec3 rho(glm::vec3 woRender, std::span<const float> uc, std::span<const glm::vec2> u) const;
	void Regularize();

	operator bool() const;
};

#endif // PIXIE_ENGINE_BSDF