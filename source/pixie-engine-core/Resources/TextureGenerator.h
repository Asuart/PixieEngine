#pragma once
#include "pch.h"
#include "Texture.h"
#include "Math/Random.h"
#include "Scene/Skybox.h"
#include "Buffer2DTexture.h"

namespace PixieEngine {

class TextureGenerator {
public:
	static Texture SSAONoiseTexture(glm::ivec2 resolution);

	static Skybox TextureSkybox(Buffer2DTexture<glm::vec3>& equirectangularTexture, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution);
	static Skybox SolidColorSkybox(glm::vec3 color, glm::ivec2 resolution = { 1, 1 }, glm::ivec2 irradianceResolution = { 1, 1 }, glm::ivec2 prefilterResolution = { 1, 1 });
	static Skybox GradientSkybox(glm::vec3 upColor, glm::vec3 botColor, glm::ivec2 resolution = { 32, 32 }, glm::ivec2 irradianceResolution = { 32, 32 }, glm::ivec2 prefilterResolution = { 32, 32 });
};

}