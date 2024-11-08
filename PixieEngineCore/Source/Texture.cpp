#include "pch.h"
#include "Texture.h"

template <>
Texture<Vec3>::Texture(int32_t width, int32_t height, uint8_t* data, int32_t numChannels)
	: resolution(width, height), pixels(width * height) {
	for (int32_t i = 0; i < pixels.size(); i++) {
		pixels[i] = Vec3(data[i * numChannels + 0], data[i * numChannels + 1], data[i * numChannels + 2]) / 255.0f;
	}
	Upload();
}

template <>
Texture<Vec4>::Texture(int32_t width, int32_t height, uint8_t* data, int32_t numChannels)
	: resolution(width, height), pixels(width * height) {
	for (int32_t i = 0; i < pixels.size(); i++) {
		pixels[i] = Vec4(data[i * numChannels + 0], data[i * numChannels + 1], data[i * numChannels + 2], data[i * numChannels + 3]) / 255.0f;
	}
	Upload();
}

template <>
void Texture<glm::fvec3>::TexImage2D() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, &pixels[0]);
}

template <>
void Texture<glm::fvec4>::TexImage2D() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
}

template <>
void Texture<Spectrum>::TexImage2D() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT_TYPE, &pixels[0]);
}

template <>
void Texture<float>::TexImage2D() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, &pixels[0]);
}
