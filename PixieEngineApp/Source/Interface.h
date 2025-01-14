#pragma once
#include "pch.h"
#include "InterfaceWindow.h"

class PixieEngineApp;

class Interface {
public:
	Interface(PixieEngineApp& app);
	~Interface();

	void Initialize();
	void Draw();

protected:
	PixieEngineApp& m_app;
	std::vector<InterfaceWindow*> m_openWindows;

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
