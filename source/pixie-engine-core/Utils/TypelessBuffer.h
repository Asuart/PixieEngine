#pragma once
#include "pch.h"

namespace PixieEngine {

class DynamicTypelessBuffer {
public:
	DynamicTypelessBuffer(size_t initialSize = 0) : m_buffer(initialSize) {}

	template<typename T>
	DynamicTypelessBuffer(std::vector<T> v) {
		int32_t size = sizeof(T);
		m_buffer.reserve(size * v.size());
		for (size_t i = 0; i < v.size(); i++) {
			Push<T>(v[i]);
		};
	}

	void* Data() { return (void*)m_buffer.data(); }
	size_t Size() const { return m_buffer.size(); }
	void Reserve(size_t size) { m_buffer.reserve(size); }
	void Resize(size_t size) { m_buffer.resize(size); }
	void Clear() { m_buffer.clear(); }

	template<typename T>
	T Read(int32_t index) {
		const int32_t size = sizeof(T);
		T value;
		uint8_t* p = (uint8_t*)&value;
		for (int32_t i = 0; i < size; i++) {
			p[i] = m_buffer[index * size + i];
		}
		return value;
	}

	template<typename T>
	void Write(int32_t index, T value) {
		const int32_t size = sizeof(T);
		const uint8_t* p = (uint8_t*)&value;
		for (int32_t i = 0; i < size; i++) {
			m_buffer[index * size + i] = p[i];
		}
	}

	template<typename T>
	void Push(T value) {
		const int32_t size = sizeof(T);
		const uint8_t* p = (uint8_t*)&value;
		for (int32_t i = 0; i < size; i++) {
			m_buffer.push_back(p[i]);
		}
	}

private:
	std::vector<uint8_t> m_buffer;
};

}