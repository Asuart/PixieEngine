#pragma once
#include "pch.h"
#include "Renderer.h"
#include "RayTracingStatistics.h"
#include "Film.h"
#include "Scene.h"
#include "Bounds.h"

class Integrator : public Renderer {
public:
	RayTracingStatistics m_stats;
	Film m_film;
	int32_t m_maxDepth = 1024;
	int32_t m_maxThreads = 2;

	virtual void SetScene(Scene* scene);
	virtual void SetResolution(const glm::ivec2& resolution) override;
	virtual void Reset() override;
	virtual void StartRender();
	virtual void StopRender();
	virtual void PerPixel(uint32_t x, uint32_t y, Sampler* sampler);
	virtual Spectrum Integrate(Ray ray, Sampler* sampler) = 0;
	uint32_t GetThreadsCount();
	uint32_t GetSamplesCount();
	float GetRenderTime();
	float GetLastSampleTime();

protected:
	Integrator(const glm::ivec2& resolution);

	Scene* m_scene = nullptr;
	bool m_isRendering = false;
	uint32_t m_samples = 1;
	int32_t m_threadsCount = 0;
	std::vector<std::thread*> m_renderThreads;
	glm::ivec2 m_tileSize = glm::ivec2(64, 64);
	std::vector<Bounds2i> m_tiles;
	std::queue<int32_t> m_tileQueue;
	std::mutex m_tileQueueMutex;
	std::chrono::microseconds m_renderStartTime;
	std::chrono::microseconds m_sampleStartTime;
	std::chrono::microseconds m_lastSampleTime;

	bool Unoccluded(uint32_t x, uint32_t y, const SurfaceInteraction& p0, const SurfaceInteraction& p1);
	void GenerateTiles();
};