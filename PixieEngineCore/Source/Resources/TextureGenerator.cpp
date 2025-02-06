#include "pch.h"
#include "TextureGenerator.h"

Texture TextureGenerator::SSAONoiseTexture(glm::ivec2 resolution) {
	std::vector<Vec3> ssaoNoise(resolution.x * resolution.y);
	for (int32_t i = 0; i < resolution.x * resolution.y; i++) {
		ssaoNoise[i] = Vec3(RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f, 0.0f);
	}
	return Texture(resolution, GL_RGBA32F, GL_RGB, GL_FLOAT, ssaoNoise.data());
}

Texture TextureGenerator::CreateCubemap(glm::ivec2 resolution) {
	GLuint cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (int32_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return Texture(resolution, cubemap);
}
