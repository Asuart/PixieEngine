#include "NoiseTexture.h"

NoiseTexture::NoiseTexture(Vec3 _scale)
	: scale(_scale) {}

Vec3 NoiseTexture::Sample(const RTInteraction& intr) const {
	return Vec3(0.5f * (1.0f + sin(scale * intr.p.z + 10.0f * noise.Turbulence(intr.p * scale))));
}