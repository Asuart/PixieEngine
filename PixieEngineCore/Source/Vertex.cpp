
#include "Vertex.h"

Vertex::Vertex(const Vec3& _p, const Vec3& _n, const Vec2& _uv)
	: p(_p), n(_n), uv(_uv) {
	for (int32_t i = 0; i < MaxBonesPerVertex; i++) {
		boneIDs[i] = -1;
		boneWeights[i] = 0.0f;
	}
}