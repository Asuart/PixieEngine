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

Texture TextureGenerator::CreateCubemap(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type) {
	GLuint cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (int32_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, resolution.x, resolution.y, 0, format, type, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return Texture(resolution, cubemap);
}

HDRISkybox TextureGenerator::Skybox(Buffer2DTexture<Vec3>& equirectangularTexture, glm::ivec2 resolution, glm::ivec2 irradianceResolution, glm::ivec2 prefilterResoution) {
	Texture envCubemap = TextureGenerator::CreateCubemap(resolution);
	Texture irradianceMap = TextureGenerator::CreateCubemap(irradianceResolution);
	Texture prefilterMap = TextureGenerator::CreateCubemap(prefilterResoution);
	GlobalRenderer::DrawCubeMap(equirectangularTexture.GetID(), resolution, envCubemap.m_id, irradianceResolution, irradianceMap.m_id, prefilterResoution, prefilterMap.m_id);
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
