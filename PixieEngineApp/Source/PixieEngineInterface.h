#pragma once
#include "pch.h"
#include "AssetsBrowserWindow.h"
#include "SceneTreeWindow.h"
#include "InspectorWindow.h"
#include "MaterialsBrowserWindow.h"
#include "RayTracingSettingsWindow.h"
#include "DefaultViewportWindow.h"
#include "RayTracingViewportWindow.h"
#include "StereoscopicViewportWindow.h"
#include "StereoscopicViewportSettingsWindow.h"
#include "VRViewportWindow.h"
#include "VRViewportSettingsWindow.h"
#include "StatsWindow.h"
#include "RendererViewportSettingsWindow.h"
#include "GPURayTracingViewportWindow.h"

class PixieEngineApp;

class PixieEngineInterface {
public:
	PixieEngineInterface(PixieEngineApp& app);
	~PixieEngineInterface();

	void Initialize();
	void Draw();

protected:
	PixieEngineApp& m_app;
	std::vector<PixieEngineInterfaceWindow*> m_openWindows;

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
