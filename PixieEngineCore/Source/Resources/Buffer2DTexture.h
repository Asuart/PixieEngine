#pragma once
#include "Texture.h"
#include "Buffer2D.h"

template<class T>
class Buffer2DTexture {
public:
	Buffer2DTexture(glm::ivec2 resolution = { 1, 1 });
	Buffer2DTexture(const Buffer2D<T>& buffer);

	GLuint GetID() const;
	const Texture& GetTexture() const;
	glm::ivec2 GetResolution() const;
	void Resize(glm::ivec2 resolution);
	void Clear();
	void SetPixel(uint32_t index, T value);
	void SetPixel(glm::ivec2 coords, T value);
	void AddPixel(uint32_t index, T value);
	void AddPixel(glm::ivec2 coords, T value);
	T GetPixel(uint32_t index) const;
	T GetPixel(glm::ivec2 coords) const;
	void Upload();
	T Sample(Vec2 coords) const;

protected:
	Buffer2D<T> m_buffer;
	Texture m_texture;
};

template<class T>
inline Buffer2DTexture<T>::Buffer2DTexture(glm::ivec2 resolution) :
	m_buffer(resolution), m_texture(resolution) {}

template<class T>
inline Buffer2DTexture<T>::Buffer2DTexture(const Buffer2D<T>& buffer) :
	m_buffer(buffer) {
	Upload();
}

template<class T>
inline GLuint Buffer2DTexture<T>::GetID() const {
	return m_texture.GetHandle();
}

template<class T>
inline const Texture& Buffer2DTexture<T>::GetTexture() const {
	return m_texture;
}

template<class T>
inline glm::ivec2 Buffer2DTexture<T>::GetResolution() const {
	return m_buffer.m_resolution;
}

template<class T>
inline void Buffer2DTexture<T>::Resize(glm::ivec2 resolution) {
	m_texture.Resize(resolution);
	m_buffer.Resize(resolution);
}

template<class T>
inline void Buffer2DTexture<T>::Clear() {
	m_buffer.Clear();
}

template<class T>
inline void Buffer2DTexture<T>::SetPixel(uint32_t index, T value) {
	if (index >= m_buffer.m_data.size()) return;
	m_buffer.m_data[index] = value;
}

template<class T>
inline void Buffer2DTexture<T>::SetPixel(glm::ivec2 coords, T value) {
	if (coords.x < 0 || coords.x >= m_buffer.m_resolution.x || coords.y < 0 || coords.y >= m_buffer.m_resolution.y) {
		return;
	}
	uint32_t index = coords.y * m_buffer.m_resolution.x + coords.x;
	m_buffer.m_data[index] = value;
}

template<class T>
inline void Buffer2DTexture<T>::AddPixel(uint32_t index, T value) {
	if (index >= m_buffer.m_data.size()) return;
	m_buffer.m_data[index] += value;
}

template<class T>
inline void Buffer2DTexture<T>::AddPixel(glm::ivec2 coords, T value) {
	if (coords.x < 0 || coords.x >= m_buffer.m_resolution.x || coords.y < 0 || coords.y >= m_buffer.m_resolution.y) {
		return;
	}
	uint32_t index = coords.y * m_buffer.m_resolution.x + coords.x;
	m_buffer.m_data[index] += value;
}

template<class T>
inline T Buffer2DTexture<T>::GetPixel(uint32_t index) const {
	if (index >= m_buffer.m_data.size()) return T();
	return m_buffer.m_data[index];
}

template<class T>
inline T Buffer2DTexture<T>::GetPixel(glm::ivec2 coords) const {
	if (coords.x < 0 || coords.x >= m_buffer.m_resolution.x || coords.y < 0 || coords.y >= m_buffer.m_resolution.y) {
		return T();
	}
	uint32_t index = coords.y * m_buffer.m_resolution.x + coords.x;
	return m_buffer.m_data[index];
}

template<>
inline void Buffer2DTexture<Float>::Upload() {
	m_texture.Upload(m_buffer.m_resolution, GL_R32F, GL_RED, GL_FLOAT, m_buffer.Data());
}

template<>
inline void Buffer2DTexture<Vec3>::Upload() {
	m_texture.Upload(m_buffer.m_resolution, GL_RGB32F, GL_RGB, GL_FLOAT, m_buffer.Data());
}

template<>
inline void Buffer2DTexture<Vec4>::Upload() {
	m_texture.Upload(m_buffer.m_resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT, m_buffer.Data());
}

template<class T>
inline T Buffer2DTexture<T>::Sample(Vec2 coords) const {
	glm::ivec2 c = Vec2(m_buffer.m_resolution) * coords;
	return GetPixel(c);
}
