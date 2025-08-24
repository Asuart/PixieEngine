#pragma once
#include "pch.h"
#include "Math/Bounds.h"
#include "Math/Random.h"
#include "Mesh.h"
#include "Rendering/MeshHandle.h"
#include "Rendering/RenderEngine.h"

namespace PixieEngine {

struct MarchingCubesChunk {
	static const int32_t cSize = 32;

	glm::vec3 position = glm::vec3(0);
	std::array<float, cSize* cSize* cSize> values = { 0 };
	MeshHandle mesh;

	Bounds3f GetBounds() const;

	float GetValue(glm::ivec3 corner) const;
	void SetValue(glm::ivec3 corner, float value);
};

class MarchingCubesTerrain {
public:
	static const int32_t cSize = 2;
	static const int32_t cWorldSize = cSize * MarchingCubesChunk::cSize;

	std::array<MarchingCubesChunk, cSize* cSize* cSize> m_chunks;

	MarchingCubesTerrain();

	const MarchingCubesChunk& GetChunk(glm::ivec3 chunk) const;
	MarchingCubesChunk& GetChunk(glm::ivec3 chunk);

private:
	void GenerateChunk(glm::ivec3 chunk);
	void UpdateChunkMesh(glm::ivec3 chunk);
	float GetCornerValue(glm::ivec3 corner) const;
	uint8_t GetCornerState(glm::ivec3 corner) const;
	uint8_t GetCellCase(glm::ivec3 cell) const;
	glm::vec3 GetPositionOnEdge(glm::ivec3 cell, int32_t edge) const;
};

}