#include "pch.h"
#include "TextureGenerator.h"
#include "Rendering/GlobalRenderer.h"

Texture TextureGenerator::SSAONoiseTexture(glm::ivec2 resolution) {
	std::vector<Vec3> ssaoNoise(resolution.x * resolution.y);
	for (int32_t i = 0; i < resolution.x * resolution.y; i++) {
		ssaoNoise[i] = Vec3(RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f, 0.0f);
	}
	return Texture(resolution, GL_RGBA32F, GL_RGB, GL_FLOAT, ssaoNoise.data());
}

HDRISkybox TextureGenerator::Skybox(Buffer2DTexture<Vec3>& equirectangularTexture, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResoution) {
	Cubemap envCubemap = Cubemap(resolution);
	Cubemap irradianceMap = Cubemap(irradianceResolution);
	Cubemap prefilterMap = Cubemap(prefilterResoution);
	GlobalRenderer::DrawCubeMap(equirectangularTexture.GetTexture(), resolution, envCubemap, irradianceResolution, irradianceMap, prefilterResoution, prefilterMap);
	return HDRISkybox(equirectangularTexture, envCubemap, irradianceMap, prefilterMap);
}

HDRISkybox TextureGenerator::SolidColorSkybox(Vec3 color, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution) {
	Buffer2DTexture<Vec3> sphericalMap(resolution);
	for (int32_t i = 0; i < resolution.x * resolution.y; i++) {
		sphericalMap.SetPixel(i, color);
	}
	sphericalMap.Upload();
	return Skybox(sphericalMap, resolution, irradianceResolution, prefilterResolution);
}

HDRISkybox TextureGenerator::GradientSkybox(Vec3 upColor, Vec3 botColor, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResolution) {
	Buffer2DTexture<Vec3> sphericalMap(resolution);
	for (int32_t y = 0; y < resolution.y; y++) {
		for (int32_t x = 0; x < resolution.x; x++) {
			sphericalMap.SetPixel({x, y}, Lerp(1.0f - (Float)y / resolution.y, upColor, botColor));
		}
	}
	sphericalMap.Upload();
	return Skybox(sphericalMap, resolution, irradianceResolution, prefilterResolution);
}
