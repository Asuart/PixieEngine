#pragma once
#include "pch.h"

class RayTracingRenderer {
public:
	RayTracingRenderer(glm::ivec2 resolution, RTScene* scene);
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
	CPURayTracer* m_rayTracer;
	RTScene* m_scene = nullptr;
};