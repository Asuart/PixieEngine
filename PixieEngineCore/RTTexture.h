#ifndef PIXIE_ENGINE_RTTEXTURE
#define PIXIE_ENGINE_RTTEXTURE

#include "pch.h"
#include "RTInteraction.h"
#include "PerlinNoise.h"

class RTTexture {
public:
	virtual glm::vec3 Sample(const RTInteraction& intr) const = 0;
};

#endif // PIXIE_ENGINE_RTTEXTURE