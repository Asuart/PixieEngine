#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ComponentRenderer.h"
#include "RayTracingViewportWindow.h"

class RayTracingSettingsWindow : public PixieEngineInterfaceWindow {
public:
	RayTracingSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;

protected:
};
