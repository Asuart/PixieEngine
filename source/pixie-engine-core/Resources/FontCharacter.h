#include "pch.h"
#include "Rendering/TextureHandle.h"

namespace PixieEngine {

struct FontCharacter {
	TextureHandle texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	uint32_t advance;
};

}