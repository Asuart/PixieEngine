#pragma once
#include "pch.h"
#include "Math/Transform.h"
#include "Resources/Material.h"

class PixieEngineEditor;

class ComponentRenderer {
public:
	static void DrawTransform(Transform& transform, bool showHead = true, bool defaultOpen = true);
	static void DrawMaterial(std::shared_ptr<Material> material, bool showHead = true, bool defaultOpen = false);
	static void DrawComponent(Component* component, bool showHead = true, bool defaultOpen = true);
};
