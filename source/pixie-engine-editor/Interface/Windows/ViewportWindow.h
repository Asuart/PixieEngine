#pragma once
#include "pch.h"
#include "PixieEngineEditor.h"
#include "InterfaceWindow.h"
#include "ViewportCameraController.h"
#include "Resources/MarchingCubes.h"

enum class RenderMode {
	Forward = 0,
	Deffered,
	COUNT
};

std::string to_string(RenderMode renderMode);

class ViewportWindow : public InterfaceWindow {
public:
	ViewportWindow(PixieEngineEditor& app, Interface& inter);

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
	MarchingCubesTerrain* m_terrain = nullptr;

	void ResetRenderer();

	friend class ViewportSettingsWindow;
};
