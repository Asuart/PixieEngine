#pragma once
#include "PixieEngineAppHeaders.h"
#include "PixieEngineApp.h"

class PixieEngineApp;

enum class RayTracingMode : uint32_t {
	RandomWalk = 0,
	SimplePathTracing,
	PathTracing,
	TestNormals,
	TestSampler,
	COUNT
};

std::string to_string(RayTracingMode mode);

class RayTracingRenderer {
public:
	glm::ivec2 m_resolution;
	glm::ivec2 m_viewportResolution;
	Integrator* m_rayTracer;
	RTScene* m_scene = nullptr;
	bool m_resizeRendererToVieport = false;

	RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, RTScene* scene);
	~RayTracingRenderer();

	void DrawFrame();
	void Reset();
	void SetScene(RTScene* scene);
	void StartRender();
	void StopRender();
	void DrawUI();
	void SetViewportSize(glm::ivec2 resolution);
	void SetRayTracingMode(RayTracingMode mode);

protected:
	PixieEngineApp* m_parent;
	RayTracingMode m_rayTracingMode = RayTracingMode::RandomWalk;
};