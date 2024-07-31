
#include "TrowbridgeReitzDistribution.h"

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(float alpha_x, float alpha_y)
	: alpha_x(alpha_x), alpha_y(alpha_y) {}

float TrowbridgeReitzDistribution::D(glm::vec3 wm) const {
	float tan2Theta = Tan2Theta(wm);
	if (std::isinf(tan2Theta)) return 0;
	float cos4Theta = Sqr(Cos2Theta(wm));
	float e = tan2Theta * (Sqr(CosPhi(wm) / alpha_x) + Sqr(SinPhi(wm) / alpha_y));
	return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Sqr(1 + e));
}

bool TrowbridgeReitzDistribution::EffectivelySmooth() const {
	return std::max(alpha_x, alpha_y) < 1e-3f;
}

float TrowbridgeReitzDistribution::G1(glm::vec3 w) const {
	return 1 / (1 + Lambda(w));
}

float TrowbridgeReitzDistribution::Lambda(glm::vec3 w) const {
	float tan2Theta = Tan2Theta(w);
	if (std::isinf(tan2Theta)) return 0;
	float alpha2 = Sqr(CosPhi(w) * alpha_x) + Sqr(SinPhi(w) * alpha_y);
	return (std::sqrt(1 + alpha2 * tan2Theta) - 1) / 2;
}

float TrowbridgeReitzDistribution::G(glm::vec3 wo, glm::vec3 wi) const {
	return 1 / (1 + Lambda(wo) + Lambda(wi));
}

float TrowbridgeReitzDistribution::D(glm::vec3 w, glm::vec3 wm) const {
	return G1(w) / AbsCosTheta(w) * D(wm) * glm::abs(glm::dot(w, wm));
}

float TrowbridgeReitzDistribution::PDF(glm::vec3 w, glm::vec3 wm) const {
	return D(w, wm);
}

glm::vec3 TrowbridgeReitzDistribution::Sample_wm(glm::vec3 w, glm::vec2 u) const {
	glm::vec3 wh = glm::normalize(glm::vec3(alpha_x * w.x, alpha_y * w.y, w.z));
	if (wh.z < 0)
		wh = -wh;

	glm::vec3 T1 = (wh.z < 0.99999f) ? glm::normalize(glm::cross(glm::vec3(0, 0, 1), wh)) : glm::vec3(1, 0, 0);
	glm::vec3 T2 = glm::cross(wh, T1);

	glm::vec2 p = SampleUniformDiskPolar(u);

	float h = std::sqrt(1 - Sqr(p.x));
	p.y = Lerp((1 + wh.z) / 2, h, p.y);

	float pz = std::sqrt(std::max<float>(0, 1 - length2(glm::vec2(p))));
	glm::vec3 nh = p.x * T1 + p.y * T2 + pz * wh;
	return glm::normalize(glm::vec3(alpha_x * nh.x, alpha_y * nh.y, std::max<float>(1e-6f, nh.z)));
}

float TrowbridgeReitzDistribution::RoughnessToAlpha(float roughness) {
	return std::sqrt(roughness);
}

void TrowbridgeReitzDistribution::Regularize() {
	if (alpha_x < 0.3f) alpha_x = Clamp(2 * alpha_x, 0.1f, 0.3f);
	if (alpha_y < 0.3f) alpha_y = Clamp(2 * alpha_y, 0.1f, 0.3f);
}