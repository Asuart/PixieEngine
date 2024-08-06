#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Interaction.h"
#include "PerlinNoise.h"

class RTTexture {
public:
	virtual Vec3 Sample(const SurfaceInteraction& intr) const = 0;
};
