#pragma once
#include "pch.h"
#include "Buffer2D.h"
#include "Texture.h"

struct RayTracingStatistics {
	static void Resize(glm::ivec2 resolution);
	static void Reset();

	static uint64_t GetTotalRays();
	//static const Buffer2D<uint64_t>& GetRays();
	static void IncrementRays();
	static uint64_t GetTotalBoxTests();
	//static const Buffer2D<uint64_t>& GetBoxTests();
	static void IncrementBoxTests();
	static uint64_t GetTotalTriangleTests();
	//static const Buffer2D<uint64_t>& GetTriangleTests();
	static void IncrementTriangleTests();
	static uint64_t GetTotalPixelSamples();
	//static const Buffer2D<uint64_t>& GetPixelSamples();
	static void IncrementPixelSamples();

	static void UploadBoxTestsTextureLinear();
	static void UploadTriangleTestsTextureLinear();

	static void BindBoxTestsTexture();
	static void BindTriangleTestsTexture();

protected:
	static glm::ivec2 m_resolution;
	static Buffer2D<uint64_t> m_rayCountBuffer;
	static Buffer2D<uint64_t> m_sampleCountBuffer;
	static Buffer2D<uint64_t> m_boxChecksBuffer;
	static Buffer2D<uint64_t> m_triangleChecksBuffer;
	static Texture<glm::fvec3> m_boxChecksTexture;
	static Texture<glm::fvec3> m_triangleChecksTexture;
};

extern thread_local uint32_t g_threadPixelCoordX;
extern thread_local uint32_t g_threadPixelCoordY;
