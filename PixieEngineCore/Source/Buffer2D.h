#pragma once
#include "pch.h"

template <typename T>
class Buffer2D {
public:

	Buffer2D(const glm::ivec2& resolution)
		: m_resolution(resolution), m_data(std::vector<T>(resolution.x * resolution.y)) {}

	void Resize(const glm::ivec2& resolution) {
		m_resolution = resolution;
		m_data.resize(resolution.x * resolution.y);
	}

	T GetValue(uint32_t index) {
		return m_data[index];
	}

	T GetValue(uint32_t x, uint32_t y) {
		return m_data[y * m_resolution.x + x];
	}

	glm::ivec2 GetResolution() {
		return m_resolution;
	}

	size_t GetSize() {
		return m_data.size();
	}

	void SetValue(uint32_t x, uint32_t y, T value) {
		m_data[y * m_resolution.x + x] = T;
	}

	T* Data() {
		return m_data.data();
	}

	void Clear() {
		std::fill(m_data.data(), m_data.data() + m_data.size(), 0);
	}

	void Increment(uint32_t x, uint32_t y) {
		m_data[y * m_resolution.x + x]++;
	}

private:
	std::vector<T> m_data;
	glm::ivec2 m_resolution;
};
