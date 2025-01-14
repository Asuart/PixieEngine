#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "SceneManager.h"

class PixieEngineApp;

class SceneTreeWindow : public InterfaceWindow {
public:
	SceneTreeWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;

protected:
	void DrawSceneTree(SceneObject* sceneObject);
	void HandleUserInput();
};
