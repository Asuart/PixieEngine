#pragma once

class PixieEngineApp;
class Interface;

class MainWindowMenu {
public:
	MainWindowMenu(PixieEngineApp& app, Interface& inter);

	void Draw();

protected:
	PixieEngineApp& m_app;
	Interface& m_interface;
};
