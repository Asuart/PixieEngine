#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ComponentRenderer.h"

class StereoscopicViewportSettingsWindow : public PixieEngineInterfaceWindow {
public:
	StereoscopicViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};
