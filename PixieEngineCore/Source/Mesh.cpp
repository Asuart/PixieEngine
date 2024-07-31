
#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<int32_t>& _indices, Material* material)
	: vertices(_vertices), indices(_indices), material(material) {}
