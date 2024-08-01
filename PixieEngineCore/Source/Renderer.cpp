#include "Renderer.h"

Renderer::Renderer(const glm::ivec2 resolution)
	: m_resolution(resolution) {}


glm::ivec2 Renderer::GetResolution() {
	return m_resolution;
}

void Renderer::SetResolution(const glm::ivec2& resolution) {
	m_resolution = resolution;
}