#pragma once
#include "pch.h"

class PixieEngineApp;
class Interface;

class InterfaceWindow {
public:
	InterfaceWindow(PixieEngineApp& app, Interface& inter);

	virtual void Initialize();
	virtual void Draw();

protected:
	PixieEngineApp& m_app;
	Interface& m_interface;
};
