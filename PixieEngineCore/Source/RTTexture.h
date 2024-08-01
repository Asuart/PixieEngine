#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTInteraction.h"
#include "PerlinNoise.h"

class RTTexture {
public:
	virtual Vec3 Sample(const RTInteraction& intr) const = 0;
};
