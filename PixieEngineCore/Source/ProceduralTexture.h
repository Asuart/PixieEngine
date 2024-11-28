#pragma once

template<typename T>
class ProceduralTexture {
public:

	virtual T Sample(Vec2 uv) = 0;
	virtual T Sample(glm::ivec2 coord) = 0;

	Texture<T> MakeStatic() {
		Texture<T> texture = Texture<T>(m_resolution);
		for (int32_t y = 0; y < m_resolution.y; y++) {
			for (int32_t x = 0; x < m_resolution.x; x++) {
				texture.m_pixels[y * m_resolution.x + x] = Sample(x, y);
			}
		}
		return texture;
	}

protected:
	glm::ivec2 m_resolution;

	ProceduralTexture(glm::ivec2 resolution) : m_resolution(resolution) {}
};

template<typename T>
class UniformTexture : public ProceduralTexture<T> {
public:
	UniformTexture(glm::ivec2 resolution, T value) :
		ProceduralTexture<T>(resolution), m_value(value) {}

	virtual T Sample(Vec2 uv) override {
		return m_value;
	}

	virtual T Sample(glm::ivec2 coord) override {
		return m_value;
	}

protected:
	T m_value;
};
