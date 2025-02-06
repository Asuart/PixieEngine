#pragma once
#include "pch.h"
#include "Texture.h"
#include "Random.h"

class TextureGenerator {
public:
	static Texture SSAONoiseTexture(glm::ivec2 resolution);
	static Texture CreateCubemap(glm::ivec2 resolution);
};