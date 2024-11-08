#pragma once
#include "pch.h"

class PixieEngineApp;
class PixieEngineInterface;

class PixieEngineInterfaceWindow {
public:
	UID id;

	PixieEngineInterfaceWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	virtual void Initialize();
	virtual void Draw();

protected:
	PixieEngineApp& m_app;
	PixieEngineInterface& m_interface;
};
