#include "pch.h"
#include "TextureGenerator.h"
#include "Rendering/RenderEngine.h"

Texture TextureGenerator::SSAONoiseTexture(glm::ivec2 resolution) {
	std::vector<glm::vec3> ssaoNoise(resolution.x * resolution.y);
	for (int32_t i = 0; i < resolution.x * resolution.y; i++) {
		ssaoNoise[i] = glm::vec3(RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f, 0.0f);
	}
	return Texture(resolution, GL_RGBA32F, GL_RGB, GL_FLOAT, ssaoNoise.data());
}

Skybox TextureGenerator::TextureSkybox(Buffer2DTexture<glm::vec3>& equirectangularTexture, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResoution) {
	Cubemap envCubemap = Cubemap(resolution);
	Cubemap irradianceMap = Cubemap(irradianceResolution);
	Cubemap prefilterMap = Cubemap(prefilterResoution);
	RenderEngine::DrawCubeMap(equirectangularTexture.GetTexture(), resolution, envCubemap, irradianceResolution, irradianceMap, prefilterResoution, prefilterMap);
	return Skybox(envCubemap, irradianceMap, prefilterMap);
}

Skybox TextureGenerator::SolidColorSkybox(glm::vec3 color, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution) {
	Buffer2DTexture<glm::vec3> sphericalMap(resolution);
	for (int32_t i = 0; i < resolution.x * resolution.y; i++) {
		sphericalMap.SetPixel(i, color);
	}
	sphericalMap.Upload();
	return Skybox(resolution, irradianceResolution, prefilterResolution);
}

Skybox TextureGenerator::GradientSkybox(glm::vec3 upColor, glm::vec3 botColor, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution) {
	Buffer2DTexture<glm::vec3> sphericalMap(resolution);
	for (int32_t y = 0; y < resolution.y; y++) {
		for (int32_t x = 0; x < resolution.x; x++) {
			sphericalMap.SetPixel({ x, y }, Lerp(1.0f - (float)y / resolution.y, upColor, botColor));
		}
	}
	sphericalMap.Upload();
	return Skybox(resolution, irradianceResolution, prefilterResolution);
}
