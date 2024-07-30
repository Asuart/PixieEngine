#ifndef PIXIE_ENGINE_MESH
#define PIXIE_ENGINE_MESH

#include "pch.h"
#include "Vertex.h"
#include "Material.h"

class Mesh {
public:
	const std::vector<Vertex> vertices;
	const std::vector<int32_t> indices;
	Material* material = nullptr;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int32_t>& indices, Material* material = nullptr);
};

#endif // PIXIE_ENGINE_MESH