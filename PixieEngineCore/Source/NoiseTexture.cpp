#include "NoiseTexture.h"

NoiseTexture::NoiseTexture(Vec3 _scale)
	: scale(_scale) {}

Vec3 NoiseTexture::Sample(const SurfaceInteraction& intr) const {
	return Vec3((Float)0.5f * ((Float)1.0f + sin(scale * intr.position.z + 10.0f * noise.Turbulence(intr.position * scale))));
}