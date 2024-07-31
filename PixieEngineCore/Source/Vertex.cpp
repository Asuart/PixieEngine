
#include "Vertex.h"

Vertex::Vertex(const glm::vec3& _p, const glm::vec3& _n, const glm::vec2& _uv)
	: p(_p), n(_n), uv(_uv) {
	for (int32_t i = 0; i < MAX_BONES_PER_VERTEX; i++) {
		boneIDs[i] = -1;
		boneWeights[i] = 0.0f;
	}
}