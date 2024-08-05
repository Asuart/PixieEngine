#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Buffer2D.h"

struct RayTracingStatistics {
	RayTracingStatistics(glm::ivec2 resolution);

	void Resize(glm::ivec2 resolution);
	void Clear();

	glm::ivec2 m_resolution;
	Buffer2D<uint64_t> m_rayStatBuffer;
	Buffer2D<uint64_t> m_boxCheckStatBuffer;
	Buffer2D<uint64_t> m_triangleCheckStatBuffer;
	Buffer2D<uint64_t> m_sampleCountBuffer;
};