#pragma once
#include "pch.h"

static const uint32_t cMaxBonesPerVertex = 4;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	std::array<int32_t, cMaxBonesPerVertex> boneIDs;
	std::array<float, cMaxBonesPerVertex> boneWeights;

	Vertex(const glm::vec3& p = glm::vec3(0), const glm::vec3& n = glm::vec3(0), const glm::vec2& uv = glm::vec2(0));

	bool AddWeight(int32_t boneID, float weigt, bool overrideSmallest = true);
};
