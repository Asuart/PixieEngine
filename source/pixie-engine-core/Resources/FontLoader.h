#pragma once
#include "pch.h"
#include "FontCharacter.h"

class FontLoader {
public:
	static std::map<char, FontCharacter> LoadDefaultFont(uint64_t fontSize);
};
