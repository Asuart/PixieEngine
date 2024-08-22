#pragma once
#include "pch.h"
#include "Buffer2D.h"
#include "Texture.h"

struct RayTracingStatistics {
	RayTracingStatistics(glm::ivec2 resolution);

	void Resize(glm::ivec2 resolution);
	void Clear();

	glm::ivec2 m_resolution;
	Buffer2D<uint64_t> m_rayCountBuffer;
	Buffer2D<uint64_t> m_sampleCountBuffer;
	Buffer2D<uint64_t> m_boxChecksBuffer;
	Buffer2D<uint64_t> m_triangleChecksBuffer;
	Texture<glm::fvec3> m_boxChecksTexture;
	Texture<glm::fvec3> m_triangleChecksTexture;

	uint64_t GetTotalRays();
	uint64_t GetTotalBoxTests();
	uint64_t GetTotalTriangleTests();

	void UploadBoxTestsTextureLinear();
	void UploadTriangleTestsTextureLinear();

	void BindBoxTestsTexture();
	void BindTriangleTestsTexture();
};