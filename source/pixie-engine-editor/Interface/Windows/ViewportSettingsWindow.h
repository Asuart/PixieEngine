#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "Interface/ComponentRenderer.h"

class ViewportSettingsWindow : public InterfaceWindow {
public:
	ViewportSettingsWindow(PixieEngineEditor& app, Interface& inter);

	void Draw() override;
};
