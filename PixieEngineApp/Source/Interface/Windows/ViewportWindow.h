#pragma once
#include "pch.h"
#include "PixieEngineApp.h"
#include "InterfaceWindow.h"
#include "ViewportCameraController.h"

enum class ViewportMode {
	Default = 0,
	VR,
	Stereoscopic,
	COUNT
};

enum class RenderMode {
	Forward = 0,
	Deffered,
	PathTracing,
	GPUPathTracing,
	COUNT
};

std::string to_string(ViewportMode viewportMode);
std::string to_string(RenderMode renderMode);

class ViewportWindow : public InterfaceWindow {
public:
	ViewportWindow(PixieEngineApp& app, Interface& inter);

	void Draw();

	ViewportMode GetViewportMode() const;
	void SetViewportMode(ViewportMode mode);
	RenderMode GetRenderMode() const;
	void SetRenderMode(RenderMode renderMode);

	// Stereoscopic Settings Access
	Float GetStereoscopicDistance() const;
	void SetStereoscopicDistance(Float distance);
	Vec3 GetStereoscopicBalance() const;
	void SetStereoscopicBalance(Vec3 balance);
	Vec3 GetStereoscopicScale() const;
	void SetStereoscopicScale(Vec3 scale);

	// VR Settings Access
	Float GetVRDistance() const;
	void SetVRDistance(Float distance);
	Float GetVRDistortion() const;
	void SetVRDistortion(Float distortion);

	const FrameBuffer& GetFrameBuffer();

protected:
	FrameBuffer m_frameBuffer;
	ForwardRenderer m_forwardRenderer;
	ForwardRenderer m_secondaryForwardRenderer;
	DefferedRenderer m_defferedRenderer;
	DefferedRenderer m_secondaryDefferedRenderer;
	PathTracingRenderer m_pathTracingRenderer;
	Shader m_vrShader;
	Shader m_stereoscopicShader;
	Camera m_viewportCamera;
	ViewportCameraController m_cameraController;
	ViewportMode m_mode = ViewportMode::Default;
	RenderMode m_renderMode = RenderMode::Deffered;
	bool m_useCameraResolution = false;

	// Stereoscopic Specific Config
	FrameBuffer m_stereoscopicFrameBuffer;
	Float m_stereoscopicDistance = 0.05f;
	Vec3 m_stereoscopicBalance = Vec3(0.15f, 0.5f, 0.85f);
	Vec3 m_stereoscopicScale = Vec3(1.0f);

	// VR Specific Config
	FrameBuffer m_vrFrameBuffer;
	Float m_vrDistance = 0.0062f;
	Float m_vrDistortion = 0.25f;

	friend class ViewportSettingsWindow;
};

