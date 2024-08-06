#include "Mesh.h"

Vertex::Vertex(const Vec3& p, const Vec3& n, const Vec2& uv)
	: position(p), normal(n), uv(uv) {
	for (int32_t i = 0; i < MaxBonesPerVertex; i++) {
		boneIDs[i] = -1;
		boneWeights[i] = 0.0f;
	}
}

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<int32_t>& _indices)
	: vertices(_vertices), indices(_indices) {}