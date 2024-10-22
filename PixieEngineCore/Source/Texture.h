#pragma once
#include "pch.h"
#include "Interaction.h"
#include "Spectrum.h"

template <typename T>
struct Texture {
	GLuint id = 0;
	glm::ivec2 resolution;
	std::vector<T> pixels;

	Texture(int32_t width, int32_t height, uint8_t* data, int32_t numChannels = 3);

	Texture(glm::ivec2 _resolution)
		: resolution(_resolution), pixels(_resolution.x * _resolution.y) {}

	~Texture() {
		glDeleteTextures(1, &id);
	}

	int32_t GetByteSize() {
		return (int32_t)pixels.size() * sizeof(T);
	}

	std::vector<T>& GetPixels() {
		return pixels;
	}

	T GetPixel(int32_t x, int32_t y) {
		uint32_t pixelIndex = y * resolution.x + x;
		if (pixelIndex >= pixels.size()) return T();
		return pixels[pixelIndex];
	}

	T GetPixel(int32_t index) {
		if (index >= pixels.size()) return T();
		return pixels[index];
	}

	T GetPixel(const glm::ivec2& coord) {
		uint32_t pixelIndex = coord.y * resolution.x + coord.x;
		if (pixelIndex >= pixels.size()) return T();
		return pixels[pixelIndex];
	}

	void SetPixel(int32_t x, int32_t y, T p) {
		uint32_t pixelIndex = y * resolution.x + x;
		if (pixelIndex >= pixels.size()) return;
		pixels[pixelIndex] = p;
	}

	void SetPixel(int32_t index, T p) {
		if (index >= pixels.size()) return;
		pixels[index] = p;
	}

	void AccumulatePixel(int32_t x, int32_t y, T p) {
		uint32_t pixelIndex = y * resolution.x + x;
		if (pixelIndex >= pixels.size()) return;
		pixels[pixelIndex] += p;
	}

	void Resize(glm::ivec2 _resolution) {
		resolution = _resolution;
		pixels.resize(resolution.x * resolution.y);
		Reset();
	}

	void Reset() {
		memset(&pixels[0], 0, GetByteSize());
	}

	void Upload() {
		if (id == 0) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glBindTexture(GL_TEXTURE_2D, id);
		TexImage2D();
	}

	void Bind(GLuint activeTexture = GL_TEXTURE0) {
		if (id == 0) {
			std::cout << "Binding not loaded texture.\n";
			return;
		}
		glActiveTexture(activeTexture);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	T Sample(const SurfaceInteraction& intr) const {
		uint32_t x = (uint32_t)(intr.uv.x * (resolution.x - 1));
		uint32_t y = (uint32_t)(intr.uv.y * (resolution.y - 1));
		uint32_t pixelIndex = y * resolution.x + x;
		if (pixelIndex >= pixels.size()) return T();
		return pixels[pixelIndex];
	}

private:
	void TexImage2D();
};
