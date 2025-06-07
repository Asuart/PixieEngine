#pragma once
#include "pch.h"
#include "Texture.h"
#include "Buffer2DTexture.h"
#include "Scene/Skybox.h"

class TextureLoader {
public:
	static Texture LoadTextureFloat(const std::filesystem::path& filePath);
	static Texture LoadTexture(const std::filesystem::path& filePath);
	static Texture LoadTextureRGB(const std::filesystem::path& filePath);
	static Texture LoadTextureRGBA(const std::filesystem::path& filePath);
	static Buffer2DTexture<float> LoadBuffer2DTextureFloat(const std::filesystem::path& filePath);
	static Buffer2DTexture<glm::vec3> LoadBuffer2DTextureRGB(const std::filesystem::path& filePath);
	static Buffer2DTexture<glm::vec4> LoadBuffer2DTextureRGBA(const std::filesystem::path& filePath);
	static Skybox LoadSkybox(const std::filesystem::path& path);
};
