#ifndef PIXIE_ENGINE_BXDF
#define PIXIE_ENGINE_BXDF

#include "pch.h"
#include "Math.h"
#include "BxDFTools.h"
#include "BSDFSample.h"

class BxDF {
public:
	virtual BxDFFlags Flags() const = 0;
	virtual glm::vec3 f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const = 0;
	virtual std::optional<BSDFSample> Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode = TransportMode::Radiance, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	virtual float PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const = 0;
	glm::vec3 rho(glm::vec3 wo, std::span<const float> uc, std::span<const glm::vec2> u2) const;
	virtual void Regularize() = 0;
};

#endif // PIXIE_ENGINE_BXDF