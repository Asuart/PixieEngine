#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"

class VRViewportWindow : public PixieEngineInterfaceWindow {
public:
	VRViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	glm::ivec2 m_viewportResolution;
	GLuint m_vrShader = 0;
	FrameBuffer* m_viewportFrameBuffer = nullptr;
	FrameBuffer* m_viewportFrameBufferLeft = nullptr;
	FrameBuffer* m_viewportFrameBufferRight = nullptr;
	float m_distance = 0.0062f;
	float m_k = 0.25f;

	void UpdateViewportResolution(glm::ivec2 resolution);

	friend class VRViewportSettingsWindow;
};
