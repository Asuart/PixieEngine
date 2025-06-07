#include "pch.h"
#include "FontLoader.h"

std::map<char, FontCharacter> FontLoader::LoadDefaultFont(uint64_t fontSize) {
	std::map<char, FontCharacter> characters = {};
	const std::string fontsPath = Globals::GetApplicationDirectory().string() + std::string("/Resources/Fonts/");
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		Log::Error("FREETYPE: Could not init FreeType Library");
		return characters;
	}
	FT_Face face;
	if (FT_New_Face(ft, (fontsPath + std::string("Arial.ttf")).c_str(), 0, &face)) {
		Log::Error("FREETYPE: Failed to load default font");
		return characters;
	}
	FT_Set_Pixel_Sizes(face, 0, (FT_UInt)fontSize);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		Log::Error("FREETYPE: Failed to Failed to load Glyph");
		return characters;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	for (int32_t c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			Log::Error("FREETYPE: Failed to Failed to load Glyph");
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FontCharacter character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(uint32_t)face->glyph->advance.x
		};
		characters.insert(std::pair<char, FontCharacter>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return characters;
}
