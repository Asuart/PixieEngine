#pragma once
#include "pch.h"
#include "PixieEngineApp.h"

class PixieEngineApp;

enum class RayTracingVisualization : uint32_t {
	Integration = 0,
	BoxChecksStatistics,
	TriangleChecksStatistics,
	COUNT
};

enum class RayTracingMode : uint32_t {
	RandomWalk = 0,
	SimplePathTracing,
	PathTracing,
	TestNormals,
	TestSampler,
	COUNT
};

std::string to_string(RayTracingVisualization mode);
std::string to_string(RayTracingMode mode);

Integrator* CreateIntegrator(RayTracingMode mode, const glm::ivec2& resolution);

class RayTracingRenderer {
public:
	glm::ivec2 m_resolution;
	glm::ivec2 m_viewportResolution;
	Integrator* m_rayTracer;
	Scene* m_scene = nullptr;
	bool m_resizeRendererToVieport = false;

	RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, Scene* scene, RayTracingMode mode = RayTracingMode::RandomWalk);
	~RayTracingRenderer();

	void DrawFrame();
	void Reset();
	void SetScene(Scene* scene);
	void StartRender();
	void StopRender();
	void DrawUI();
	void SetViewportSize(glm::ivec2 resolution);
	void SetVisualizationMode(RayTracingVisualization mode);
	void SetRayTracingMode(RayTracingMode mode);

protected:
	PixieEngineApp* m_parent;
	RayTracingMode m_rayTracingMode;
	RayTracingVisualization m_visualizationMode = RayTracingVisualization::Integration;
};