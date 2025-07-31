#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "Interface/ComponentRenderer.h"

class InspectorWindow : public InterfaceWindow {
public:
	InspectorWindow(PixieEngineEditor& app, Interface& inter);

	void Draw() override;
};
