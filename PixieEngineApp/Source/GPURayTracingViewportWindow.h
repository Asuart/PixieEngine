#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"

class PixieEngineApp;
class PixieEngineInterface;

struct AlignedTriangle {
	glm::fvec3 p0 = glm::fvec3(0.0f);
	int32_t material = 0;
	glm::fvec3 p1 = glm::fvec3(0.0f);
	float area = 0;
	glm::fvec3 p2 = glm::fvec3(0.0f);
	float padding = 0;

	AlignedTriangle() = default;
	AlignedTriangle(Triangle tri);
};

struct AlignedMaterial {
	Vec4 albedo;
	Vec4 emission;

	AlignedMaterial() = default;
	AlignedMaterial(Material& material);
};

struct AlignedNode {
	Vec4 pMin;
	Vec4 pMax;
	int32_t secondChildOffset;
	int16_t nPrimitives;
	int16_t axis;
	int64_t padding = 0;

	AlignedNode() = default;
	AlignedNode(BVHNode& node);
};

class GPURayTracingViewportWindow : public PixieEngineInterfaceWindow {
public:
	GPURayTracingViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	FrameBuffer* m_viewportFrameBuffer = nullptr;
	SceneSnapshot* m_sceneSnapshot = nullptr;
	ComputeShader m_shader;
	GLuint m_resultTexture = 0;
	glm::ivec2 m_resultTextureResolution = { 1280, 720 };
	glm::ivec2 m_viewportResolution = { 1280, 720 };
	GLuint m_trianglesBuffer = 0;
	int32_t m_numTriangles = 0;
	GLuint m_materialsBuffer = 0;
	int32_t m_samples = 0;

	void UpdateViewportResolution(glm::ivec2 resolution);
	void UpdateScene();
};