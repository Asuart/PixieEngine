#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"

class DefaultViewportWindow : public PixieEngineInterfaceWindow {
public:
	DefaultViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	glm::ivec2 m_viewportResolution;
	FrameBuffer* m_viewportFrameBuffer = nullptr;

	void UpdateViewportResolution(glm::ivec2 resolution);

	friend class RendererViewportSettingsWindow;
};
