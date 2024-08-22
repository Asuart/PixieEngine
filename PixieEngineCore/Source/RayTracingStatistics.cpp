#include "pch.h"
#include "RayTracingStatistics.h"

RayTracingStatistics::RayTracingStatistics(glm::ivec2 resolution)
	: m_resolution(resolution), m_rayCountBuffer(resolution), m_sampleCountBuffer(resolution), m_boxChecksBuffer(resolution), m_triangleChecksBuffer(resolution),
	m_triangleChecksTexture(resolution), m_boxChecksTexture(resolution) {}

void RayTracingStatistics::Resize(glm::ivec2 resolution) {
	m_rayCountBuffer.Resize(resolution);
	m_boxChecksBuffer.Resize(resolution);
	m_triangleChecksBuffer.Resize(resolution);
	m_sampleCountBuffer.Resize(resolution);
	m_triangleChecksTexture.Resize(resolution);
	m_boxChecksTexture.Resize(resolution);
}

void RayTracingStatistics::Clear() {
	m_rayCountBuffer.Clear();
	m_boxChecksBuffer.Clear();
	m_triangleChecksBuffer.Clear();
	m_sampleCountBuffer.Clear();
	m_triangleChecksTexture.Reset();
	m_boxChecksTexture.Reset();
}

uint64_t RayTracingStatistics::GetTotalRays() {
	uint64_t total = 0;
	for (size_t i = 0; i < m_rayCountBuffer.GetSize(); i++) {
		total += m_rayCountBuffer.GetValue((uint32_t)i);
	}
	return total;
}

uint64_t RayTracingStatistics::GetTotalBoxTests() {
	uint64_t total = 0;
	for (size_t i = 0; i < m_boxChecksBuffer.GetSize(); i++) {
		total += m_boxChecksBuffer.GetValue((uint32_t)i);
	}
	return total;
}

uint64_t RayTracingStatistics::GetTotalTriangleTests() {
	uint64_t total = 0;
	for (size_t i = 0; i < m_triangleChecksBuffer.GetSize(); i++) {
		total += m_triangleChecksBuffer.GetValue((uint32_t)i);
	}
	return total;
}

void RayTracingStatistics::UploadBoxTestsTextureLinear() {
	std::vector<glm::fvec3>& pixels = m_boxChecksTexture.GetPixels();
	std::vector<uint64_t> values(pixels.size());
	uint64_t max = 0;
	for (size_t i = 0; i < pixels.size(); i++) {
		values[i] = m_boxChecksBuffer.GetValue((uint32_t)i) / (m_sampleCountBuffer.GetValue((uint32_t)i) + 1);
		if (values[i] > max) max = values[i];
	}
	if (max == 0) max = 1;
	for (size_t i = 0; i < pixels.size(); i++) {
		pixels[i] = Lerp((values[i] / (double)max), glm::fvec3(0.0f), glm::fvec3(1.0f));
	}
	m_boxChecksTexture.Upload();
}

void RayTracingStatistics::UploadTriangleTestsTextureLinear() {
	std::vector<glm::fvec3>& pixels = m_triangleChecksTexture.GetPixels();
	std::vector<uint64_t> values(pixels.size());
	uint64_t max = 0;
	for (size_t i = 0; i < pixels.size(); i++) {
		values[i] = m_triangleChecksBuffer.GetValue((uint32_t)i) / (m_sampleCountBuffer.GetValue((uint32_t)i) + 1);
		if (values[i] > max) max = values[i];
	}
	if (max == 0) max = 1;
	for (size_t i = 0; i < pixels.size(); i++) {
		pixels[i] = Lerp((values[i] / (double)max), glm::fvec3(0.0f), glm::fvec3(1.0f));
	}
	m_triangleChecksTexture.Upload();
}

void RayTracingStatistics::BindBoxTestsTexture() {
	m_boxChecksTexture.Bind(0);
}

void RayTracingStatistics::BindTriangleTestsTexture() {
	m_triangleChecksTexture.Bind(0);
}