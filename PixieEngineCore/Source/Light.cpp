#include "Light.h"

Light::Light(Spectrum emission) 
	: m_emission(emission) {}

Spectrum Light::Le(const Ray& ray) {
	return m_emission;
}

LightLiSample::LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight)
	: L(L), wi(wi), pdf(pdf), pLight(pLight) {}

AreaLight::AreaLight(Shape* shape, const Material* material)
	: shape(shape), material(material) {}

DiffuseAreaLight::DiffuseAreaLight(Shape* shape, const Material* material)
	: AreaLight(shape, material) {};

Spectrum DiffuseAreaLight::L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const {
	if (!twoSided && glm::dot(n, w) < 0.0f) {
		return Spectrum();
	}
	return material->GetEmission();
}

std::optional<LightLiSample> DiffuseAreaLight::SampleLi(SurfaceInteraction intr, Vec2 u) const {
	std::optional<ShapeSample> ss = shape->Sample(u);
	if (!ss || ss->pdf == 0 || length2(ss->intr.position - intr.position) == 0) {
		return {};
	}

	Vec3 wi = glm::normalize(ss->intr.position - intr.position);
	Spectrum Le = L(ss->intr.position, ss->intr.normal, ss->intr.uv, wi);
	if (!Le) {
		return {};
	}

	Float solidAnglePDF = shape->SolidAngle(intr.position) * Inv2Pi;

	return LightLiSample(Le, wi, ss->pdf / solidAnglePDF, ss->intr);
}

Float DiffuseAreaLight::PDF_Li(SurfaceInteraction ctx, Vec3 wi, bool allowIncompletePDF) const {
	return shape->PDF(ctx, wi);
}

UniformLightSampler::UniformLightSampler(const std::vector<AreaLight*>& _lights)
	: lights(_lights) {}

std::optional<SampledLight> UniformLightSampler::Sample(Float u) const {
	if (lights.empty()) {
		return {};
	}
	int32_t lightIndex = std::min<int32_t>((int32_t)(u * lights.size()), (int32_t)lights.size() - 1);
	return SampledLight{ lights[lightIndex], 1.0f / lights.size() };
}

Float UniformLightSampler::PMF(const Light* light) const {
	if (lights.empty()) {
		return 0.0f;
	}
	return 1.0f / lights.size();
}

Float UniformLightSampler::PMF(const SurfaceInteraction& ctx, const Light* light) const {
	return PMF(light); 
}