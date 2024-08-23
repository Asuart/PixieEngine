#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Spectrum spectrum, float scale, const Transform& transform)
	: Light(LightType::DeltaDirection, transform, nullptr), m_spectrum(spectrum), m_scale(scale) {}

Spectrum DirectionalLight::Phi() const {
	return m_scale * m_spectrum * Pi * Sqr(m_sceneRadius);
}

std::optional<LightLiSample> DirectionalLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	Vec3 wi = glm::normalize(m_transform.ApplyVector(Vec3(0.0f, 0.0f, 1.0f)));
	Vec3 pOutside = context.position + wi * (2.0f * m_sceneRadius);
	return LightLiSample(m_scale * m_spectrum, wi, 1.0f, SurfaceInteraction(pOutside, nullptr));
}

void DirectionalLight::Preprocess(const Bounds3f& sceneBounds) {
	sceneBounds.BoundingSphere(&m_sceneCenter, &m_sceneRadius);
}