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
	Texture(glm::ivec2 _resolution);
	~Texture();

	int32_t GetByteSize();
	std::vector<T>& GetPixels();
	T GetPixel(int32_t x, int32_t y);
	T GetPixel(int32_t index);
	T GetPixel(glm::ivec2 coord);
	void SetPixel(int32_t x, int32_t y, T p);
	void SetPixel(int32_t index, T p);
	void SetPixel(glm::ivec2 coord, T p);
	void AccumulatePixel(int32_t x, int32_t y, T p);
	void AccumulatePixel(int32_t index, T p);
	void AccumulatePixel(glm::ivec2 coord, T p);
	void Resize(glm::ivec2 _resolution);
	void Reset();
	void Upload();
	void Bind(GLuint activeTexture = GL_TEXTURE0);

	void FreeCPUData();
	void FreeGPUData();

protected:
	void TexImage2D();
};

template<typename T>
Texture<T>::Texture(glm::ivec2 _resolution) :
	resolution(_resolution), pixels(_resolution.x* _resolution.y) {}

template<typename T>
Texture<T>::~Texture() {
	glDeleteTextures(1, &id);
}

template<typename T>
int32_t Texture<T>::GetByteSize() {
	return (int32_t)pixels.size() * sizeof(T);
}

template<typename T>
std::vector<T>& Texture<T>::GetPixels() {
	return pixels;
}

template<typename T>
T Texture<T>::GetPixel(int32_t x, int32_t y) {
	uint32_t pixelIndex = y * resolution.x + x;
	if (pixelIndex >= pixels.size()) return T();
	return pixels[pixelIndex];
}

template<typename T>
T Texture<T>::GetPixel(int32_t index) {
	if (index >= pixels.size()) return T();
	return pixels[index];
}

template<typename T>
T Texture<T>::GetPixel(glm::ivec2 coord) {
	uint32_t pixelIndex = coord.y * resolution.x + coord.x;
	if (pixelIndex >= pixels.size()) return T();
	return pixels[pixelIndex];
}

template<typename T>
void Texture<T>::SetPixel(int32_t x, int32_t y, T p) {
	uint32_t pixelIndex = y * resolution.x + x;
	if (pixelIndex >= pixels.size()) return;
	pixels[pixelIndex] = p;
}

template<typename T>
void Texture<T>::SetPixel(int32_t index, T p) {
	if (index >= pixels.size()) return;
	pixels[index] = p;
}

template<typename T>
void Texture<T>::SetPixel(glm::ivec2 coord, T p) {
	uint32_t index = coord.y * resolution.x + coord.x;
	if (index >= pixels.size()) return;
	pixels[index] = p;
}

template<typename T>
void Texture<T>::AccumulatePixel(int32_t x, int32_t y, T p) {
	uint32_t index = y * resolution.x + x;
	if (index >= pixels.size()) return;
	pixels[index] += p;
}

template<typename T>
void Texture<T>::AccumulatePixel(int32_t index, T p) {
	if (index >= pixels.size()) return;
	pixels[index] += p;
}

template<typename T>
void Texture<T>::AccumulatePixel(glm::ivec2 coord, T p) {
	uint32_t index = coord.y * resolution.x + coord.x;
	if (index >= pixels.size()) return;
	pixels[index] += p;
}

template<typename T>
void Texture<T>::Resize(glm::ivec2 _resolution) {
	resolution = _resolution;
	pixels.resize(resolution.x * resolution.y);
	Reset();
}

template<typename T>
void Texture<T>::Reset() {
	memset(&pixels[0], 0, GetByteSize());
}

template<typename T>
void Texture<T>::Upload() {
	if (id == 0) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, id);
	TexImage2D();
}

template<typename T>
void Texture<T>::Bind(GLuint activeTexture) {
	if (id == 0) {
		std::cout << "Binding not loaded texture.\n";
		return;
	}
	glActiveTexture(activeTexture);
	glBindTexture(GL_TEXTURE_2D, id);
}

template<typename T>
void Texture<T>::FreeCPUData() {
	resolution = glm::ivec2(0, 0);
	pixels.resize(0);
}

template<typename T>
void Texture<T>::FreeGPUData() {
	glDeleteTextures(1, &id);
	id = 0;
}
