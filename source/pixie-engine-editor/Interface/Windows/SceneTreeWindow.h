#pragma once
#include "pch.h"
#include "Scene/SceneObject.h"
#include "InterfaceWindow.h"

class PixieEngineEditor;

class SceneTreeWindow : public InterfaceWindow {
public:
	SceneTreeWindow(PixieEngineEditor& app, Interface& inter);

	void Draw() override;

protected:
	void DrawSceneTree(SceneObject* sceneObject);
	void HandleUserInput();
};
