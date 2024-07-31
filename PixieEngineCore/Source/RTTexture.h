#pragma once
#include "pch.h"
#include "RTInteraction.h"
#include "PerlinNoise.h"

class RTTexture {
public:
	virtual glm::vec3 Sample(const RTInteraction& intr) const = 0;
};
