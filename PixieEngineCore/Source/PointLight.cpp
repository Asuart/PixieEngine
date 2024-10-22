#include "pch.h"
#include "PointLight.h"

PointLight::PointLight(Spectrum spectrum, Float scale, const Transform& transform, MediumInterface* mediumInterface)
	: Light(LightType::DeltaPosition, transform, mediumInterface), m_spectrum(spectrum), m_scale(scale) {}

Spectrum PointLight::Phi() const {
    return 4 * Pi * m_scale * m_spectrum;
}

std::optional<LightLiSample> PointLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
    Vec3 p = m_transform.ApplyPoint(Vec3(0.0f, 0.0f, 0.0f));
    Vec3 wi = glm::normalize(p - context.position);
    Spectrum Li = m_scale * m_spectrum / glm::length2(p - context.position);
    return LightLiSample(Li, wi, 1, SurfaceInteraction(p, m_mediumInterface));
}

Float PointLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
    return 0;
}

std::optional<LightLeSample> PointLight::SampleLe(Vec2 u1, Vec2 u2) const {
    Vec3 p = m_transform.ApplyPoint(Vec3(0.0f));
    Ray ray(p, SampleUniformSphere(u1), m_mediumInterface->outside);
    return LightLeSample(m_scale * m_spectrum, ray, 1, UniformSpherePDF);
}

void PointLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
    *pdfPos = 0;
    *pdfDir = UniformSpherePDF;
}

void PointLight::SampleLePDF(const SurfaceInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
    std::cout << "SampleLePDF Shouldn't be called for non-area lights.\n";
}

Bounds3f PointLight::Bounds() const {
    return Bounds3f();
}
