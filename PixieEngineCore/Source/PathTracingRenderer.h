#pragma once
#include "pch.h"
#include "RayTracers.h"
#include "FrameBuffer.h"
#include "Film.h"
#include "SceneManager.h"
#include "GlobalRenderer.h"

class PathTracingRenderer {
public:
	FrameBuffer m_frameBuffer;
	Camera m_camera;
	std::shared_ptr<SceneSnapshot> m_sceneSnapshot;

	PathTracingRenderer();
	~PathTracingRenderer();

	void DrawFrame();
	void Reset();
	void StartRender(std::shared_ptr<SceneSnapshot> sceneSnapshot, const Camera& camera);
	void PauseRender();
	void StopRender();

	RayTracingMode GetRayTracingMode();
	void SetRayTracingMode(RayTracingMode mode);
	int32_t GetMaxRenderThreads();
	void SetMaxRenderThreads(int32_t count);
	uint32_t GetSamplesCount() const;
	Float GetRenderTime() const;
	Float GetLastSampleTime() const;

protected:
	const glm::ivec2 m_tileSize = glm::ivec2(32, 32);
	RayTracingMode m_rayTracingMode = RayTracingMode::Naive;
	RayTracer* m_rayTracer;
	Film m_film;
	bool m_isRendering = false;
	bool m_isPaused = false;
	int32_t m_samples = 1;
	int32_t m_maxThreads = 1;
	int32_t m_samplesPerPixel = 8192;
	int32_t m_threadsCount = 0;
	std::vector<std::thread*> m_renderThreads;
	std::vector<Bounds2i> m_tiles;
	std::queue<int32_t> m_tileQueue;
	std::mutex m_tileQueueMutex;
	std::chrono::microseconds m_renderStartTime = std::chrono::microseconds(0);
	std::chrono::microseconds m_sampleStartTime = std::chrono::microseconds(0);
	std::chrono::microseconds m_lastSampleTime = std::chrono::microseconds(0);
	Texture<Float> m_boxTestsTexture;
	Texture<Float> m_shapeTestsTexture;
	Texture<Vec3> m_normalTexture;
	Texture<Float> m_depthTexture;

	void GenerateTiles();
	void ResetTileQueue();
	void PerPixel(uint32_t x, uint32_t y, Sampler* sampler);
	CameraSample GetCameraSample(uint32_t x, uint32_t y, const FilmFilter* filter, Sampler* sampler);
};