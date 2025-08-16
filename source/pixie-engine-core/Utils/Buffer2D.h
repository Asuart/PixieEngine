#pragma once
#include "pch.h"

namespace PixieEngine {

template <typename T>
class Buffer2D {
public:
	std::vector<T> m_data;
	glm::ivec2 m_resolution;

	Buffer2D(const glm::ivec2& resolution) :
		m_resolution(resolution), m_data(std::vector<T>(resolution.x* resolution.y)) {
	}

	inline void Resize(const glm::ivec2& resolution) {
		m_resolution = resolution;
		m_data.resize(resolution.x * resolution.y);
	}

	inline void Clear() {
		std::fill(m_data.data(), m_data.data() + m_data.size(), (T)0);
	}

	T GetValue(uint32_t index) const {
		return m_data[index];
	}

	T GetValue(glm::ivec2 coords) const {
		return m_data[coords.y * m_resolution.x + coords.x];
	}

	void SetValue(uint32_t index, T value) {
		m_data[index] = value;
	}

	void SetValue(glm::ivec2 coords, T value) {
		m_data[coords.y * m_resolution.x + coords.x] = value;
	}

	inline int32_t GetWidth() const {
		return m_resolution.x;
	}

	inline int32_t GetHeight() const {
		return m_resolution.y;
	}

	inline glm::ivec2 GetResolution() const {
		return m_resolution;
	}

	inline size_t GetSize() const {
		return m_data.size();
	}

	inline size_t GetByteSize() const {
		return sizeof(T) * GetSize();
	}

	inline T* Data() {
		return m_data.data();
	}
};

}