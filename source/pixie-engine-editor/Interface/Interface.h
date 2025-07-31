#pragma once
#include "pch.h"
#include "Windows/InterfaceWindow.h"
#include "MainWindowMenu.h"

class PixieEngineEditor;

class Interface {
public:
	Interface(PixieEngineEditor& app);
	~Interface();

	void Initialize();
	void Draw();

protected:
	PixieEngineEditor& m_app;
	std::vector<InterfaceWindow*> m_openWindows;
	MainWindowMenu m_menu;

public:
	template<typename T>
	std::vector<T*> GetWindowsOfType() {
		std::vector<T*> result;
		for (size_t i = 0; i < m_openWindows.size(); i++) {
			T* cast = dynamic_cast<T*>(m_openWindows[i]);
			if (cast) {
				result.push_back(cast);
			}
		}
		return result;
	}
};
