#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Renderer.h"
#include "RayTracingStatistics.h"
#include "Film.h"
#include "RTScene.h"
#include "Bounds.h"

class Integrator : public Renderer {
public:
	Film m_film;
	int32_t m_maxDepth = 1024;
	uint32_t m_samples = 1;

	virtual void SetScene(RTScene* scene);
	virtual void SetResolution(const glm::ivec2& resolution) override;
	virtual void Reset() override;
	virtual void StartRender();
	virtual void StopRender();
	virtual void PerPixel(uint32_t x, uint32_t y);
	virtual Vec3 Integrate(Ray ray) = 0;

protected:
	Integrator(const glm::ivec2& resolution);

	RTScene* m_scene = nullptr;
	RayTracingStatistics m_stats;
	bool m_isRendering = false;
	int32_t m_threadsCount = 0;
	int32_t m_maxThreads = 8;
	std::vector<std::thread*> m_renderThreads;
	glm::ivec2 m_tileSize = glm::ivec2(64, 64);
	std::vector<Bounds2i> m_tiles;
	std::queue<int32_t> m_tileQueue;
	std::mutex m_tileQueueMutex;

	bool Unoccluded(uint32_t x, uint32_t y, const SurfaceInteraction& p0, const SurfaceInteraction& p1);
	void GenerateTiles();
};