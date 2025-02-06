#pragma once
#include "pch.h"
#include "Texture.h"
#include "Math/Random.h"

class TextureGenerator {
public:
	static Texture SSAONoiseTexture(glm::ivec2 resolution);
	static Texture CreateCubemap(glm::ivec2 resolution);
};
