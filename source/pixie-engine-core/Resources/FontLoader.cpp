#include "pch.h"
#include "FontLoader.h"
#include "Debug/Log.h"
#include "EngineConfig.h"
#include "Utils/Buffer2D.h"
#include "Rendering/RenderEngine.h"

namespace PixieEngine {

std::map<char, FontCharacter> FontLoader::LoadDefaultFont(uint64_t fontSize) {
	std::map<char, FontCharacter> characters = {};

	const std::string fontsPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Fonts/");

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

	for (int32_t c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			Log::Error("FREETYPE: Failed to Failed to load Glyph");
			continue;
		}

		glm::ivec2 resolution = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
		Buffer2D<float> texture(resolution);
		for (int32_t y = 0, pixelIndex = 0; y < face->glyph->bitmap.rows; y++) {
			for (int32_t x = 0; x < face->glyph->bitmap.width; x++, pixelIndex++) {
				uint8_t value = face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x];
				float remappedValue = static_cast<float>(value) / 255.0f;
				texture.SetValue(pixelIndex, remappedValue);
			}
		}

		TextureHandle textureHandle = RenderEngine::LoadTexture(texture);

		FontCharacter character = {
			textureHandle,
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

}