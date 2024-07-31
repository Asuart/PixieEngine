#pragma once
#include "Vertex.h"
#include "Material.h"

class Mesh {
public:
	const std::vector<Vertex> vertices;
	const std::vector<int32_t> indices;
	Material* material = nullptr;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices, Material* material = nullptr);
};
