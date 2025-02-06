#include "pch.h"
#include "TrowbridgeReitzDistribution.h"

TrowbridgeReitzDistribution::TrowbridgeReitzDistribution(Float alpha_x, Float alpha_y) :
	alpha_x(alpha_x), alpha_y(alpha_y) {
	if (!EffectivelySmooth()) {
		alpha_x = std::max<Float>(alpha_x, 1e-4f);
		alpha_y = std::max<Float>(alpha_y, 1e-4f);
	}
}

Float TrowbridgeReitzDistribution::D(Vec3 wm) const {
	Float tan2Theta = Tan2Theta(wm);
	if (std::isinf(tan2Theta)) {
		return 0;
	}
	Float cos4Theta = Sqr(Cos2Theta(wm));
	if (cos4Theta < 1e-16f) {
		return 0;
	}
	Float e = tan2Theta * (Sqr(CosPhi(wm) / alpha_x) + Sqr(SinPhi(wm) / alpha_y));
	return 1 / (Pi * alpha_x * alpha_y * cos4Theta * Sqr(1 + e));
}

bool TrowbridgeReitzDistribution::EffectivelySmooth() const {
	return glm::max(alpha_x, alpha_y) < 1e-3f;
}

Float TrowbridgeReitzDistribution::G1(Vec3 w) const {
	return 1.0f / (1.0f + Lambda(w));
}

Float TrowbridgeReitzDistribution::Lambda(Vec3 w) const {
	Float tan2Theta = Tan2Theta(w);
	if (std::isinf(tan2Theta)) {
		return 0;
	}
	Float alpha2 = Sqr(CosPhi(w) * alpha_x) + Sqr(SinPhi(w) * alpha_y);
	return (std::sqrt(1 + alpha2 * tan2Theta) - 1) / 2;
}

Float TrowbridgeReitzDistribution::G(Vec3 wo, Vec3 wi) const {
	return 1 / (1 + Lambda(wo) + Lambda(wi));
}

Float TrowbridgeReitzDistribution::D(Vec3 w, Vec3 wm) const {
	return G1(w) / AbsCosTheta(w) * D(wm) * AbsDot(w, wm);
}

Float TrowbridgeReitzDistribution::PDF(Vec3 w, Vec3 wm) const {
	return D(w, wm);
}

Vec3 TrowbridgeReitzDistribution::Sample_wm(Vec3 w, Vec2 u) const {
	Vec3 wh = glm::normalize(Vec3(alpha_x * w.x, alpha_y * w.y, w.z));
	if (wh.z < 0) {
		wh = -wh;
	}

	Vec3 T1 = (wh.z < 0.99999f) ? glm::normalize(glm::cross(Vec3(0, 0, 1), wh)) : Vec3(1, 0, 0);
	Vec3 T2 = glm::cross(wh, T1);

	Vec2 p = SampleUniformDiskPolar(u);

	Float h = std::sqrt(1 - Sqr(p.x));
	p.y = Lerp((1 + wh.z) / 2, h, p.y);

	Float pz = std::sqrt(std::max<Float>(0, 1 - glm::length2(Vec2(p))));
	Vec3 nh = p.x * T1 + p.y * T2 + pz * wh;
	return glm::normalize(Vec3(alpha_x * nh.x, alpha_y * nh.y, std::max<Float>(1e-6f, nh.z)));
}

Float TrowbridgeReitzDistribution::RoughnessToAlpha(Float roughness) {
	return glm::sqrt(roughness);
}

void TrowbridgeReitzDistribution::Regularize() {
	if (alpha_x < 0.3f) alpha_x = Clamp(2 * alpha_x, 0.1f, 0.3f);
	if (alpha_y < 0.3f) alpha_y = Clamp(2 * alpha_y, 0.1f, 0.3f);
}