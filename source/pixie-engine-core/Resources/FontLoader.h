#pragma once
#include "pch.h"
#include "FontCharacter.h"

namespace PixieEngine {

class FontLoader {
public:
	static std::map<char, FontCharacter> LoadDefaultFont(uint64_t fontSize);
};

}
