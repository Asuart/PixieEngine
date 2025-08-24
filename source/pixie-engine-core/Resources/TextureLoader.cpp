#include "pch.h"
#include "TextureLoader.h"
#include "EngineConfig.h"
#include "Debug/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace PixieEngine {

Buffer2D<float> TextureLoader::LoadTextureFloat(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2D<float>({ 0, 0 });
		}
	}
	Buffer2D<float> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = data[i];
	}
	stbi_image_free(data);
	return buffer;
}

Buffer2D<glm::vec3> TextureLoader::LoadTextureRGB(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2D<glm::vec3>({ 0, 0 });
		}
	}
	Buffer2D<glm::vec3> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = glm::vec3(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
	}
	stbi_image_free(data);
	return buffer;
}

Buffer2D<glm::vec4> TextureLoader::LoadTextureRGBA(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2D<glm::vec4>({ 0 , 0 });
		}
	}
	Buffer2D<glm::vec4> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = glm::vec4(data[i * 4 + 0], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
	}
	stbi_image_free(data);
	return buffer;
}

}