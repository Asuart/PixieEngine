#include "pch.h"

namespace PixieEngine {

struct FontCharacter {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	uint32_t advance;
};

}