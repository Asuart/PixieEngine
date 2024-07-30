#include "pch.h"
#include "Light.h"

LightLiSample::LightLiSample(const glm::vec3& L, glm::vec3 wi, float pdf, const RTInteraction& pLight)
	: L(L), wi(wi), pdf(pdf), pLight(pLight) {}

DiffuseAreaLight::DiffuseAreaLight(Shape* shape, glm::vec3 emit, float scale)
	: shape(shape), Lemit(emit), scale(scale) {};

glm::vec3 DiffuseAreaLight::L(glm::vec3 p, glm::vec3 n, glm::vec2 uv, glm::vec3 w) const {
	if (!twoSided && glm::dot(n, w) < 0)
		return glm::vec3(0.f);
	return scale * Lemit;
}

std::optional<LightLiSample> DiffuseAreaLight::SampleLi(RTInteraction intr, glm::vec2 u) const {
	std::optional<ShapeSample> ss = shape->Sample(u);
	if (!ss || ss->pdf == 0 || glm::length2(ss->intr.p - intr.p) == 0)
		return {};

	glm::vec3 wi = glm::normalize(ss->intr.p - intr.p);
	glm::vec3 Le = L(ss->intr.p, ss->intr.n, ss->intr.uv, wi);
	if (Le == glm::vec3(0))
		return {};

	float solidAnglePDF = shape->SolidAngle(intr.p) * Inv2Pi;

	return LightLiSample(Le, wi, ss->pdf / solidAnglePDF, ss->intr);
}

UniformLightSampler::UniformLightSampler(const std::vector<DiffuseAreaLight>& _lights)
	: lights(_lights) {}

std::optional<SampledLight> UniformLightSampler::Sample(float u) const {
	if (lights.empty())
		return {};
	int lightIndex = std::min<int>(u * lights.size(), lights.size() - 1);
	return SampledLight{ lights[lightIndex], 1.f / lights.size() };
}

float UniformLightSampler::PMF() const {
	if (lights.empty())
		return 0;
	return 1.f / lights.size();
}