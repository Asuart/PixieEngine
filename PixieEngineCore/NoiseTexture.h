#ifndef PIXIE_ENGINE_NOISE_TEXTURE
#define PIXIE_ENGINE_NOISE_TEXTURE

#include "pch.h"
#include "RTTexture.h"

class NoiseTexture : public RTTexture {
public:
	NoiseTexture(glm::vec3 _scale = glm::vec3(1.0));
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	PerlinNoise noise;
	glm::vec3 scale;
};

#endif // PIXIE_ENGINE_NOISE_TEXTURE