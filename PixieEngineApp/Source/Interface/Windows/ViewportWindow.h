#pragma once
#include "pch.h"
#include "PixieEngineApp.h"
#include "InterfaceWindow.h"
#include "ViewportCameraController.h"

enum class RenderMode {
	Forward = 0,
	Deffered,
	COUNT
};

std::string to_string(RenderMode renderMode);

class ViewportWindow : public InterfaceWindow {
public:
	ViewportWindow(PixieEngineApp& app, Interface& inter);

	void Draw();

	RenderMode GetRenderMode() const;
	void SetRenderMode(RenderMode renderMode);
	AntiAliasing GetAntiAliasing() const;
	void SetAntiAliasing(AntiAliasing mode);

	const FrameBuffer& GetFrameBuffer();

protected:
	FrameBuffer m_frameBuffer;
	Renderer* m_renderer = nullptr;
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	RenderMode m_renderMode = RenderMode::Forward;

	void ResetRenderer();

	friend class ViewportSettingsWindow;
};
