#pragma once
#include "pch.h"
#include "InterfaceWindow.h"

class PixieEngineApp;
class Interface;

class StatsWindow : public InterfaceWindow {
public:
	StatsWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;
};