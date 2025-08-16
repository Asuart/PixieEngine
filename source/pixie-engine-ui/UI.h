#pragma once
#include "pch.h"
#include "UIWindow.h"
#include "Windowing/MainWindow.h"
#include "Windowing/MainWindowOpenGL.h"
#include "Windowing/MainWindowVulkan.h"

namespace PixieEngine {

class UI {
public:
	UI(MainWindow* window, bool docking);
	~UI();

	void HandleEvent(const Event& event);
	void AddWindow(UIWindow* window);
	void Draw();

protected:
	MainWindow* m_window;
	std::vector<UIWindow*> m_windows;
	bool m_isDocking;

public:
	template<typename T>
	std::vector<T*> GetWindowsOfType() {
		std::vector<T*> result;
		for (size_t i = 0; i < m_windows.size(); i++) {
			T* cast = dynamic_cast<T*>(m_windows[i]);
			if (cast) {
				result.push_back(cast);
			}
		}
		return result;
	}
};

}