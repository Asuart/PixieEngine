#include "NoiseTexture.h"

NoiseTexture::NoiseTexture(glm::vec3 _scale)
	: scale(_scale) {}

glm::vec3 NoiseTexture::Sample(const RTInteraction& intr) const {
	return glm::vec3(0.5f * (1.0f + sin(scale * intr.p.z + 10.0f * noise.Turbulence(intr.p * scale))));
}