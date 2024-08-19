#pragma once
#include "RTTexture.h"
#include "PerlinNoise.h"

class NoiseTexture : public RTTexture {
public:
	NoiseTexture(Vec3 _scale = Vec3(1.0));
	Vec3 Sample(const SurfaceInteraction& intr) const override;

private:
	PerlinNoise noise;
	Vec3 scale;
};
