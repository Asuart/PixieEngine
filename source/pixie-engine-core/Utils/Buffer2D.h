#pragma once
#include "pch.h"

template <typename T>
class Buffer2D {
public:
	std::vector<T> m_data;
	glm::ivec2 m_resolution;

	Buffer2D(const glm::ivec2& resolution) :
		m_resolution(resolution), m_data(std::vector<T>(resolution.x* resolution.y)) {}

	void Resize(const glm::ivec2& resolution) {
		m_resolution = resolution;
		m_data.resize(resolution.x * resolution.y);
	}

	virtual void Clear() {
		std::fill(m_data.data(), m_data.data() + m_data.size(), (T)0);
	}

	T GetValue(uint32_t index) const {
		return m_data[index];
	}

	T GetValue(glm::ivec2 coords) const {
		return m_data[coords.y * m_resolution.x + coords.x];
	}

	T GetValue(uint32_t x, uint32_t y) const {
		return m_data[y * m_resolution.x + x];
	}

	void SetValue(uint32_t index, T value) {
		m_data[index] = value;
	}

	void SetValue(uint32_t x, uint32_t y, T value) {
		m_data[y * m_resolution.x + x] = value;
	}

	int32_t GetWidth() const {
		return m_resolution.x;
	}

	int32_t GetHeight() const {
		return m_resolution.y;
	}

	glm::ivec2 GetResolution() const {
		return m_resolution;
	}

	size_t GetSize() const {
		return m_data.size();
	}

	size_t GetByteSize() const {
		return sizeof(T) * GetSize();
	}

	std::vector<T> CopyRow(int32_t rowIndex) const {
		std::vector<T> row(m_resolution.x);
		for (int32_t i = 0; i < m_resolution.x; i++) {
			row[i] = m_data[rowIndex * m_resolution.x + i];
		}
		return row;
	}

	T* Data() {
		return m_data.data();
	}
};
