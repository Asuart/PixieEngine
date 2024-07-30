#include "pch.h"
#include "Film.h"

Film::Film(glm::ivec2 resolution)
	: m_resolution(resolution), m_pixelSize(glm::vec2(1.0f) / (glm::vec2)resolution) {
	glm::vec2 min(-1.0), max(1.0);
	std::vector<Vertex> vertices = {
		{Vertex(glm::vec3(min.x, min.y, 0))},
		{Vertex(glm::vec3(min.x, max.y, 0))},
		{Vertex(glm::vec3(max.x, max.y, 0))},
		{Vertex(glm::vec3(max.x, min.y, 0))},
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
	texture->AccumulatePixel(x, y, color);
}

glm::vec2 Film::GetUV(int32_t x, int32_t y) const {
	return glm::vec2((float)x / m_resolution.x, (float)y / m_resolution.y);
}

glm::vec2 Film::GetUV(int32_t x, int32_t y, const glm::vec2& u) const {
	return glm::vec2((float)x / m_resolution.x, (float)y / m_resolution.y) + m_pixelSize * u;
}