#include "pch.h"
#include "PixieEngineInterfaceWindow.h"
#include "PixieEngineApp.h"
#include "PixieEngineInterface.h"

PixieEngineInterfaceWindow::PixieEngineInterfaceWindow(PixieEngineApp& app, PixieEngineInterface& inter) 
	: m_app(app), m_interface(inter) {}

void PixieEngineInterfaceWindow::Initialize() {}

void PixieEngineInterfaceWindow::Draw() {}
