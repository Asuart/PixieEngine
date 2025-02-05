#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"

class InspectorWindow : public InterfaceWindow {
public:
	InspectorWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;
};
