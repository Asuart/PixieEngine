#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ComponentRenderer.h"

class VRViewportSettingsWindow : public PixieEngineInterfaceWindow {
public:
	VRViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};
