#pragma once
#include "pch.h"

class PixieEngineEditor;
class Interface;

class InterfaceWindow {
public:
	InterfaceWindow(PixieEngineEditor& app, Interface& inter);

	virtual void Initialize();
	virtual void Draw();

protected:
	PixieEngineEditor& m_app;
	Interface& m_interface;
};
