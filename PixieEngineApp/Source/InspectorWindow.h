#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "ComponentRenderer.h"

class InspectorWindow : public PixieEngineInterfaceWindow {
public:
	InspectorWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};
