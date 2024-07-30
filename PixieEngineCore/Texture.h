#ifndef PIXIE_ENGINE_TEXTURE
#define PIXIE_ENGINE_TEXTURE

#include "pch.h"

template <typename T>
struct Texture {
	GLuint id = 0;
	glm::ivec2 resolution;
	std::vector<T> pixels;

	Texture(glm::ivec2 _resolution)
		: resolution(_resolution), pixels(_resolution.x* _resolution.y) {}

	int32_t ByteSize() {
		return pixels.size() * sizeof(T);
	}

	T GetPixel(int32_t x, int32_t y) {
		int32_t pixelIndex = y * resolution.x + x;
		return pixels[pixelIndex];
	}

	void SetPixel(int32_t x, int32_t y, T p) {
		int32_t pixelIndex = y * resolution.x + x;
		pixels[pixelIndex] = p;
	}

	void AccumulatePixel(int32_t x, int32_t y, T p) {
		int32_t pixelIndex = y * resolution.x + x;
		pixels[pixelIndex] += p;
	}

	void Resize(glm::ivec2 _resolution) {
		resolution = _resolution;
		pixels.resize(resolution.x * resolution.y);
		Reset();
	}

	void Reset() {
		memset(&pixels[0], 0, ByteSize());
	}

	void Upload() {
		if (id == 0) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

private:
	void TexImage2D();
};



#endif // PIXIE_ENGINE_TEXTURE