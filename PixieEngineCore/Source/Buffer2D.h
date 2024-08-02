#pragma once
#include "PixieEngineCoreHeaders.h"

template <typename T>
class Buffer2D {
public:

	Buffer2D(const glm::ivec2& size)
		: m_size(size), m_data(std::vector<T>(size.x * size.y)) {}

	void Resize(const glm::ivec2& size) {
		m_size = size;
		m_data.resize(size.x * size.y);
	}

	T GetValue(uint32_t x, uint32_t y) {
		return m_data[y * m_size.x + x];
	}

	void SetValue(uint32_t x, uint32_t y, T value) {
		m_data[y * m_size.x + x] = T;
	}

	T* Data() {
		return m_data.data();
	}

	void Clear() {
		std::fill(m_data.data(), m_data.data() + m_data.size(), 0);
	}

private:
	std::vector<T> m_data;
	glm::ivec2 m_size;
};
