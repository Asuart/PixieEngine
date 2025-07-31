#pragma once

class PixieEngineEditor;
class Interface;

class MainWindowMenu {
public:
	MainWindowMenu(PixieEngineEditor& app, Interface& inter);

	void Draw();

protected:
	PixieEngineEditor& m_app;
	Interface& m_interface;
};
