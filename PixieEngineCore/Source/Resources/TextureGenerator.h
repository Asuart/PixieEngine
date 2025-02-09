#pragma once
#include "pch.h"
#include "Texture.h"
#include "Math/Random.h"
#include "Scene/Skyboxes.h"

class TextureGenerator {
public:
	static Texture SSAONoiseTexture(glm::ivec2 resolution);
	static HDRISkybox Skybox(Buffer2DTexture<Vec3>& equirectangularTexture, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution);
	static HDRISkybox SolidColorSkybox(Vec3 color, glm::ivec2 resolution = { 1, 1 }, glm::ivec2 irradianceResolution = { 1, 1 }, glm::ivec2 prefilterResolution = { 1, 1 });
	static HDRISkybox GradientSkybox(Vec3 upColor, Vec3 botColor, glm::ivec2 resolution = { 32, 32 }, glm::ivec2 irradianceResolution = { 32, 32 }, glm::ivec2 prefilterResolution = { 32, 32 });
};
