#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ComponentRenderer.h"

class RendererViewportSettingsWindow : public PixieEngineInterfaceWindow {
public:
	RendererViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};
