#include "pch.h"
#include "RayTracingStatistics.h"

RayTracingStatistics::RayTracingStatistics(glm::ivec2 resolution)
	: m_resolution(resolution), m_rayStatBuffer(resolution), m_boxCheckStatBuffer(resolution), m_triangleCheckStatBuffer(resolution), m_sampleCountBuffer(resolution) {}

void RayTracingStatistics::Resize(glm::ivec2 resolution) {
	m_rayStatBuffer.Resize(resolution);
	m_boxCheckStatBuffer.Resize(resolution);
	m_triangleCheckStatBuffer.Resize(resolution);
	m_sampleCountBuffer.Resize(resolution);
}

void RayTracingStatistics::Clear() {
	m_rayStatBuffer.Clear();
	m_boxCheckStatBuffer.Clear();
	m_triangleCheckStatBuffer.Clear();
	m_sampleCountBuffer.Clear();
}