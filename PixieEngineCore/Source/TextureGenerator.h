#pragma once
#include "pch.h"
#include "Texture.h"
#include "Random.h"

class TextureGenerator {
public:
	static GLuint SSAONoiseTexture(glm::ivec2 resolution);
};