#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"
#include "RayTracingViewportWindow.h"

class RayTracingViewportWindow : public PixieEngineInterfaceWindow {
public:
	RayTracingViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;
	glm::ivec2 GetResolution();
	void SetResolution(const glm::ivec2& resolution);
	uint32_t GetMaxThreads();
	void SetMaxThreads(int32_t maxThreads);
	uint32_t GetThreadsCount();
	void Reset();
	void StartRender();
	void StopRender();
	uint32_t GetSamplesCount() const;
	float GetRenderTime();
	float GetLastSampleTime();
	CameraSample GetCameraSample(uint32_t x, uint32_t y, const FilmFilter* filter, Sampler* sampler);
	bool IsRendering();
	void SetVisualizationMode(RayTracingVisualization mode);
	void SetRayTracingMode(RayTracingMode mode);
	void ResetCamera();

protected:
	RayTracingViewportWindow* m_activeWindow = nullptr;
	RayTracingMode m_rayTracingMode = RayTracingMode::Naive;
	RayTracer* m_rayTracer;
	RayTracingVisualization m_visualizationMode = RayTracingVisualization::LightAccumulation;
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	FrameBuffer* m_viewportFrameBuffer = nullptr;
	FrameBuffer* m_cameraFrameBuffer = nullptr;
	Film* m_film = nullptr;
	glm::ivec2 m_viewportResolution;
	bool m_isRendering = false;
	uint32_t m_samples = 1;
	int32_t m_threadsCount = 0;
	int32_t m_maxThreads = 1;
	std::vector<std::thread*> m_renderThreads;
	const glm::ivec2 m_tileSize = glm::ivec2(128, 128);
	std::vector<Bounds2i> m_tiles;
	std::queue<int32_t> m_tileQueue;
	std::mutex m_tileQueueMutex;
	std::chrono::microseconds m_renderStartTime = std::chrono::microseconds(0);
	std::chrono::microseconds m_sampleStartTime = std::chrono::microseconds(0);
	std::chrono::microseconds m_lastSampleTime = std::chrono::microseconds(0);
	int32_t m_spp = 0;
	int32_t m_waveStart = 0;
	int32_t m_waveEnd = 0;
	int32_t m_nextWaveSize = 0;
	int32_t m_maxWaveSize = 1;
	bool m_resizeToViewport = false;
	Texture<Float> m_boxTestsTexture;
	Texture<Float> m_shapeTestsTexture;
	Texture<Vec3> m_normalTexture;
	Texture<Float> m_depthTexture;

	void GenerateTiles();
	void PerPixel(uint32_t x, uint32_t y, Sampler* sampler);
	void UpdateViewportResolution(glm::ivec2 resolution);

	friend class RayTracingSettingsWindow;
};
