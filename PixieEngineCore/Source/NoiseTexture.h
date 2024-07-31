#pragma once
#include "RTTexture.h"
#include "PerlinNoise.h"

class NoiseTexture : public RTTexture {
public:
	NoiseTexture(glm::vec3 _scale = glm::vec3(1.0));
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	PerlinNoise noise;
	glm::vec3 scale;
};
