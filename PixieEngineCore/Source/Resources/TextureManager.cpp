#include "pch.h"
#include "TextureManager.h"

std::map<GLuint, std::atomic<uint32_t>> TextureManager::textureCounters;
std::map<GLuint, std::atomic<uint32_t>> TextureManager::msTextureCounters;
std::map<GLuint, std::atomic<uint32_t>> TextureManager::cubemapCounters;

bool TextureManager::IsValidTexture(GLuint textureID) {
	return textureCounters.contains(textureID);
}

bool TextureManager::IsValidMSTexture(GLuint msTextureID) {
	return msTextureCounters.contains(msTextureID);
}

bool TextureManager::IsValidCubemap(GLuint cubemapID) {
	return cubemapCounters.contains(cubemapID);
}

uint32_t TextureManager::GetActiveTexturesCount() {
	return (uint32_t)textureCounters.size();
}

uint32_t TextureManager::GetActiveMSTexturesCount() {
	return (uint32_t)msTextureCounters.size();
}

uint32_t TextureManager::GetActiveCubemapsCount() {
	return (uint32_t)cubemapCounters.size();
}

uint32_t TextureManager::GetActiveUnitsCount() {
	return (uint32_t)(textureCounters.size() + msTextureCounters.size() + cubemapCounters.size());
}

void TextureManager::FreeTextures() {
	for (auto it = textureCounters.begin(); it != textureCounters.end(); it++) {
		if (it->second > 0) glDeleteTextures(1, &it->first);
	}
	textureCounters.clear();
	for (auto it = msTextureCounters.begin(); it != msTextureCounters.end(); it++) {
		if (it->second > 0) glDeleteTextures(1, &it->first);
	}
	msTextureCounters.clear();
	for (auto it = cubemapCounters.begin(); it != cubemapCounters.end(); it++) {
		if(it->second > 0) glDeleteTextures(1, &it->first);
	}
	cubemapCounters.clear();
}
