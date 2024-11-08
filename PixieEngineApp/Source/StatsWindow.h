#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"

class PixieEngineApp;
class PixieEngineInterface;

class StatsWindow : public PixieEngineInterfaceWindow {
public:
	StatsWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;
};