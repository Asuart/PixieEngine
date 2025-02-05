#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"

class MaterialsBrowserWindow : public InterfaceWindow {
public:
	MaterialsBrowserWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;
};
