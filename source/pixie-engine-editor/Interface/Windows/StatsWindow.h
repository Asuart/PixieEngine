#pragma once
#include "pch.h"
#include "InterfaceWindow.h"

class PixieEngineEditor;
class Interface;

class StatsWindow : public InterfaceWindow {
public:
	StatsWindow(PixieEngineEditor& app, Interface& inter);

	void Draw() override;
};