#pragma once
#include "pch.h"

class TextureManager {
public:
	static bool IsValidTexture(GLuint textureID);
	static bool IsValidMSTexture(GLuint msTextureID);
	static bool IsValidCubemap(GLuint cubemapID);

	static uint32_t GetActiveTexturesCount();
	static uint32_t GetActiveMSTexturesCount();
	static uint32_t GetActiveCubemapsCount();
	static uint32_t GetActiveUnitsCount();

	static void FreeTextures();

protected:
	static std::map<GLuint, std::atomic<uint32_t>> textureCounters;
	static std::map<GLuint, std::atomic<uint32_t>> msTextureCounters;
	static std::map<GLuint, std::atomic<uint32_t>> cubemapCounters;

	friend class Texture;
	friend class MSTexture;
	friend class Cubemap;
};
