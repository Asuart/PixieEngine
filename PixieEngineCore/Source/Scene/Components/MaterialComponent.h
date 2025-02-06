#pragma once
#include "Material.h"
#include "Component.h"

class SceneObject;

class MaterialComponent : public Component {
public:
	MaterialComponent(SceneObject* parent, Material* material);

	Material* GetMaterial();
	void SetMaterial(Material* material);

protected:
	Material* m_material = nullptr;
};
