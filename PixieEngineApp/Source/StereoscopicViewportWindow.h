#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"

class StereoscopicViewportWindow : public PixieEngineInterfaceWindow {
public:
	StereoscopicViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	glm::ivec2 m_viewportResolution;
	GLuint m_stereoscopicShader = 0;
	FrameBuffer* m_viewportFrameBuffer = nullptr;
	FrameBuffer* m_viewportFrameBufferLeft = nullptr;
	FrameBuffer* m_viewportFrameBufferRight = nullptr;
	float m_distance = 0.05f;
	float m_redBalance = 0.15f;
	float m_redScale = 1.0f;
	float m_greenBalance = 0.5f;
	float m_greenScale = 0.75f;
	float m_blueBalance = 0.85f;
	float m_blueScale = 1.0f;

	void UpdateViewportResolution(glm::ivec2 resolution);

	friend class StereoscopicViewportSettingsWindow;
};
