#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"

class ViewportSettingsWindow : public InterfaceWindow {
public:
	ViewportSettingsWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;
};
