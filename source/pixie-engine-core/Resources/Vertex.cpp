#include "pch.h"
#include "Vertex.h"

Vertex::Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv) :
	position(p), normal(n), uv(uv) {
	std::fill(boneIDs.begin(), boneIDs.end(), -1);
	std::fill(boneWeights.begin(), boneWeights.end(), 0.0f);
}

bool Vertex::AddWeight(int32_t boneID, float weight, bool overrideSmallest) {
	if (weight == 0) return false;

	for (uint32_t i = 0; i < cMaxBonesPerVertex; i++) {
		if (boneIDs[i] == -1) {
			boneIDs[i] = boneID;
			boneWeights[i] = weight;
			return false;
		}
	}

	if (!overrideSmallest) {
		return false;
	}

	int32_t minIndex = 0;
	float min = 0.0f;
	for (int32_t i = 0; i < cMaxBonesPerVertex; i++) {
		if (boneWeights[i] < min) {
			min = boneWeights[i];
			minIndex = i;
		}
	}
	if (weight > min) {
		boneIDs[minIndex] = boneID;
		boneWeights[minIndex] = weight;
		return true;
	}
	return false;
}
