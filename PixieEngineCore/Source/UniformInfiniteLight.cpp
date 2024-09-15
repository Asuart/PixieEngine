#include "pch.h"
#include "UniformInfiniteLight.h"

UniformInfiniteLight::UniformInfiniteLight(Spectrum spectrum, Float scale, const Transform& transform)
	: Light(LightType::Infinite, transform, nullptr), m_spectrum(spectrum), m_scale(scale) {}

Spectrum UniformInfiniteLight::Phi() const {
    return 4 * Pi * Pi * Sqr(m_sceneRadius) * m_scale * m_spectrum;
}

std::optional<LightLiSample> UniformInfiniteLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
    if (allowIncompletePDF) {
        return {};
    }
    Vec3 wi = SampleUniformSphere(u);
    Float pdf = UniformSpherePDF;
    return LightLiSample(m_scale * m_spectrum, wi, pdf, SurfaceInteraction(context.position + wi * (Float)2 * m_sceneRadius, m_mediumInterface));
}

Float UniformInfiniteLight::SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF) const {
    return allowIncompletePDF ? 0 : UniformSpherePDF;
}

Spectrum UniformInfiniteLight::Le(const Ray& ray) const {
	return m_scale * m_spectrum;
}

std::optional<LightLeSample> UniformInfiniteLight::SampleLe(Vec2 u1, Vec2 u2) const {
    Vec3 w = SampleUniformSphere(u1);

    Frame wFrame = Frame::FromZ(-w);
    Vec2 cd = SampleUniformDiskConcentric(u2);
    Vec3 pDisk = m_sceneCenter + m_sceneRadius * wFrame.FromLocal(Vec3(cd.x, cd.y, 0));
    Ray ray(pDisk + m_sceneRadius * -w, w);

    Float pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
    Float pdfDir = UniformSpherePDF;

    return LightLeSample(m_scale * m_spectrum, ray, pdfPos, pdfDir);
}

void UniformInfiniteLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
    *pdfDir = UniformSpherePDF;
    *pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
}

void UniformInfiniteLight::Preprocess(const Bounds3f& sceneBounds) {
    sceneBounds.BoundingSphere(&m_sceneCenter, &m_sceneRadius);
}
