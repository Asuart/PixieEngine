#include "pch.h"
#include "Film.h"

Film::Film(glm::ivec2 resolution)
	: m_resolution(resolution), m_pixelSize(Vec2(1.0f) / (Vec2)resolution) {
	Vec2 min(0.0), max(1.0);
	std::vector<Vertex> vertices = {
		{Vertex(Vec3(min.x, min.y, 0))},
		{Vertex(Vec3(min.x, max.y, 0))},
		{Vertex(Vec3(max.x, max.y, 0))},
		{Vertex(Vec3(max.x, min.y, 0))},
	};
	std::vector<int32_t> indices = {
		0, 1, 2,
		0, 2, 3
	};
	mesh = new OGLMesh(Mesh(vertices, indices));
	mesh->shader = CompileShader(QUAD_VERTEX_SHADER_SOURCE, QUAD_FRAGMENT_SHADER_SOURCE);
	texture = new Texture<glm::vec4>(resolution);
	texture->Upload();
}

void Film::Reset() {
	texture->Reset();
}

void Film::Resize(glm::ivec2 resolution) {
	m_resolution = resolution;
	texture->Resize(resolution);
}

void Film::SetPixel(int32_t x, int32_t y, const glm::vec4& color) {
	texture->SetPixel(x, y, color);
}

void Film::AddPixel(int32_t x, int32_t y, const glm::vec4& color) {
	texture->AccumulatePixel(x, y, glm::clamp(color, 0.0f, 1000.0f));
}

Vec2 Film::GetUV(int32_t x, int32_t y) const {
	return Vec2((Float)x / m_resolution.x, (Float)y / m_resolution.y);
}

Vec2 Film::GetUV(int32_t x, int32_t y, const Vec2& u) const {
	return Vec2((Float)x / m_resolution.x, (Float)y / m_resolution.y) + m_pixelSize * u;
}