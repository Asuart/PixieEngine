#pragma once
#include "pch.h"
#include "Utils/Buffer2D.h"
#include "Scene/Skybox.h"

namespace PixieEngine {

class TextureLoader {
public:
	static Buffer2D<float> LoadTextureFloat(const std::filesystem::path& filePath);
	static Buffer2D<glm::vec3> LoadTextureRGB(const std::filesystem::path& filePath);
	static Buffer2D<glm::vec4> LoadTextureRGBA(const std::filesystem::path& filePath);
};

}