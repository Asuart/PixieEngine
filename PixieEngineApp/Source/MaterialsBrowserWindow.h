#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"

class MaterialsBrowserWindow : public PixieEngineInterfaceWindow {
public:
	MaterialsBrowserWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};
