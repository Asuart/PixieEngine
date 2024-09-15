#include "pch.h"
#include "SpotLight.h"

SpotLight::SpotLight(Spectrum spectrum, Float scale, Float totalWidth, Float falloffStart, const Transform& transform, MediumInterface* mediumInterface)
	: Light(LightType::DeltaPosition, transform, mediumInterface), m_spectrum(spectrum), m_scale(scale), m_cosFalloffStart(falloffStart), m_cosFalloffEnd(totalWidth) {}

Spectrum SpotLight::Phi() const {
    return m_scale * m_spectrum * (Float)2 * Pi * ((1.0f - m_cosFalloffStart) + (m_cosFalloffStart - m_cosFalloffEnd) / (Float)2);
}

std::optional<LightLiSample> SpotLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
    Vec3 p = m_transform.ApplyPoint(Vec3(0.0f));
    Vec3 wi = glm::normalize(p - context.position);
    Vec3 wLight = glm::normalize(m_transform.ApplyInverseVector(-wi));
    Spectrum Li = I(wLight) / length2(p - context.position);

    if (!Li) return {};
    return LightLiSample(Li, wi, 1.0f, SurfaceInteraction(p, m_mediumInterface));
}

Float SpotLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
    return 0;
}

Spectrum SpotLight::I(Vec3 w) const {
    return SmoothStep(CosTheta(w), m_cosFalloffEnd, m_cosFalloffStart) * m_scale * m_spectrum;
}