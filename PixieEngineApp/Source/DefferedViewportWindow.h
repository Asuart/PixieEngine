#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ViewportCameraController.h"

class DefferedViewportWindow : public PixieEngineInterfaceWindow {
public:
	DefferedViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	bool m_useCameraResolution = false;
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	glm::ivec2 m_viewportResolution;

	void UpdateViewportResolution(glm::ivec2 resolution);

	friend class RendererViewportSettingsWindow;
};
