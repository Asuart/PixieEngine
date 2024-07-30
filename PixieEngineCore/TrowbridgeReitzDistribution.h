#ifndef PIXIE_ENGINE_TROWBRIDGE_REITZ_DISTRIBUTION
#define PIXIE_ENGINE_TROWBRIDGE_REITZ_DISTRIBUTION

#include "pch.h"
#include "Math.h"

class TrowbridgeReitzDistribution {
public:
	TrowbridgeReitzDistribution(float alpha_x, float alpha_y);

	float D(glm::vec3 wm) const;
	bool EffectivelySmooth() const;
	float G1(glm::vec3 w) const;
	float Lambda(glm::vec3 w) const;
	float G(glm::vec3 wo, glm::vec3 wi) const;
	float D(glm::vec3 w, glm::vec3 wm) const;
	float PDF(glm::vec3 w, glm::vec3 wm) const;
	glm::vec3 Sample_wm(glm::vec3 w, glm::vec2 u) const;
	static float RoughnessToAlpha(float roughness);
	void Regularize();

private:
	float alpha_x, alpha_y;
};

#endif // PIXIE_ENGINE_TROWBRIDGE_REITZ_DISTRIBUTION