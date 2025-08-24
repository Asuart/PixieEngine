#pragma once
#include "pch.h"
#include "TextureEnums.h"

namespace PixieEngine {

struct TextureHandle {
	uint32_t id;
	glm::ivec2 resolution;
	TextureFormat internalFormat;
};

}