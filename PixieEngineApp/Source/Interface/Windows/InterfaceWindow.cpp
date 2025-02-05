#include "pch.h"
#include "InterfaceWindow.h"
#include "PixieEngineApp.h"
#include "Interface.h"

InterfaceWindow::InterfaceWindow(PixieEngineApp& app, Interface& inter) :
	m_app(app), m_interface(inter) {}

void InterfaceWindow::Initialize() {}

void InterfaceWindow::Draw() {}
