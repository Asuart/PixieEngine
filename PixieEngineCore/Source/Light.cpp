#include "pch.h"
#include "Light.h"

bool IsDeltaLight(LightType type) {
	return (type == LightType::DeltaPosition || type == LightType::DeltaDirection);
}

LightSampleContext::LightSampleContext(const Vec3& position, const Vec3& normal)
	: position(position), normal(normal) {}

LightSampleContext::LightSampleContext(const SurfaceInteraction& intr)
	: position(intr.position), normal(intr.normal) {}

LightLiSample::LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight)
	: L(L), wi(wi), pdf(pdf), pLight(pLight) {}

LightLeSample::LightLeSample(Spectrum spectrum, Ray ray, Float pPosition, Float pDirection)
	: spectrum(spectrum), ray(ray), pPosition(pPosition), pDirection(pDirection) {}

Light::Light(LightType type, const Transform& transform, const MediumInterface* mediumInterface)
	: m_type(type), m_transform(transform), m_mediumInterface(mediumInterface) {}

Spectrum Light::L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const {
	return Spectrum();
}

Spectrum Light::Le(const Ray& ray) const {
	return Spectrum();
}

LightType Light::Type() const {
	return m_type;
}

void Light::Preprocess(const Bounds3f& sceneBounds) {}
