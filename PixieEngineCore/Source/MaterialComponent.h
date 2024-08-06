#pragma once
#include "Material.h"
#include "Component.h"

class MaterialComponent : public Component {
public:
	MaterialComponent(Material* material, const SceneObject* parent);

	const Material* GetMaterial() const;

protected:
	Material* material = nullptr;
};