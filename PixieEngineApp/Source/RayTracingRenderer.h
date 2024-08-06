#pragma once
#include "PixieEngineAppHeaders.h"
#include "PixieEngineApp.h"

class PixieEngineApp;

class RayTracingRenderer {
public:
	RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, RTScene* scene);
	~RayTracingRenderer();

	void DrawFrame();
	void Reset();
	void SetScene(RTScene* scene);
	void StartRender();
	void StopRender();
	void DrawUI();
	void SetViewportSize(glm::ivec2 resolution);

	glm::ivec2 m_resolution;
	glm::ivec2 m_viewportResolution;
	Integrator* m_rayTracer;
	RTScene* m_scene = nullptr;

protected:
	PixieEngineApp* m_parent;
};