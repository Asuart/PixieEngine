#pragma once
#include "pch.h"
#include "Vertex.h"
#include "Math/Bounds.h"

namespace PixieEngine {

struct Mesh {
	std::vector<Vertex> m_vertices;
	std::vector<int32_t> m_indices;

	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices);

	glm::vec3 GetCenter() const;
	Bounds3f GetBounds() const;
};

}