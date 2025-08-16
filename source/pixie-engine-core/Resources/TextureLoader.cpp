#include "pch.h"
#include "TextureLoader.h"
#include "TextureGenerator.h"
#include "EngineConfig.h"
#include "Debug/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace PixieEngine {

Texture TextureLoader::LoadTexture(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	Log::Message("Loading texture: %s", filePath.string().c_str());

	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 0);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture();
		}
	}
	switch (nrChannels) {
	case 1: {
		Texture texture({ width, height }, GL_R32F, GL_RED, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	case 3: {
		Texture texture({ width, height }, GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	case 4: {
		Texture texture({ width, height }, GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	default:
		Log::Error("Only 1, 3 and 4 channel textures are suported");
		stbi_image_free(data);
		return Texture();
	}
}

Texture TextureLoader::LoadTextureFloat(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_R32F, GL_RED, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Texture TextureLoader::LoadTextureRGB(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Texture TextureLoader::LoadTextureRGBA(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Buffer2DTexture<float> TextureLoader::LoadBuffer2DTextureFloat(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2DTexture<float>();
		}
	}
	Buffer2D<float> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = data[i];
	}
	stbi_image_free(data);
	return Buffer2DTexture<float>(buffer);
}

Buffer2DTexture<glm::vec3> TextureLoader::LoadBuffer2DTextureRGB(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2DTexture<glm::vec3>();
		}
	}
	Buffer2D<glm::vec3> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = glm::vec3(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
	}
	stbi_image_free(data);
	return Buffer2DTexture<glm::vec3>(buffer);
}

Buffer2DTexture<glm::vec4> TextureLoader::LoadBuffer2DTextureRGBA(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		std::filesystem::path fullPath = EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			Log::Error("Failed to load texture");
			return Buffer2DTexture<glm::vec4>();
		}
	}
	Buffer2D<glm::vec4> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = glm::vec4(data[i * 4 + 0], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
	}
	stbi_image_free(data);
	return Buffer2DTexture<glm::vec4>(buffer);
}

Skybox TextureLoader::LoadSkybox(const std::filesystem::path& path) {
	Buffer2DTexture<glm::vec3> sphericalMap = LoadBuffer2DTextureRGB(EngineConfig::GetApplicationDirectory().string() + std::string("/Resources/Skymaps/") + path.string());
	return TextureGenerator::TextureSkybox(sphericalMap, { 512, 512 }, { 32, 32 }, { 128, 128 });
}

}