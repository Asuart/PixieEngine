#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"

class PixieEngineApp;

class SceneTreeWindow : public PixieEngineInterfaceWindow {
public:
	SceneTreeWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Draw() override;

protected:
	void DrawSceneTree(SceneObject* sceneObject);
	void HandleUserInput();
};
