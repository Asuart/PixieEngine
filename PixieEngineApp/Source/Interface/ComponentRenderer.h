#pragma once
#include "pch.h"

class PixieEngineApp;

class ComponentRenderer {
public:
	static void DrawTransform(Transform& transform, bool showHead = true, bool defaultOpen = true);
	static void DrawMaterial(std::shared_ptr<Material> material, bool showHead = true, bool defaultOpen = false);
	static void DrawComponent(Component* component, bool showHead = true, bool defaultOpen = true);
};
