#pragma once
#include "pch.h"

class PixieEngineApp;

class PixieEngineInterface {
public:
	PixieEngineInterface(PixieEngineApp& app);
	~PixieEngineInterface();

	void Draw();

protected:
	PixieEngineApp& m_app;

	void DrawSettingsWindow();
	void DrawViewportWindow();
	void DrawMaterialsWindow();
	void DrawSceneWindow();
	void DrawInspectorWindow();

	void DrawSceneTree(SceneObject* sceneObject);
	void DrawTransform(Transform& transform);

	friend class PixieEngineApp;
};