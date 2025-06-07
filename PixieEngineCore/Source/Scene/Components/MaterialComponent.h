#pragma once
#include "REsources/Material.h"
#include "Component.h"

class SceneObject;

class MaterialComponent : public Component {
public:
	MaterialComponent(SceneObject* parent, std::shared_ptr<Material> material);

	std::shared_ptr<Material> GetMaterial() const;
	void SetMaterial(std::shared_ptr<Material> material);

protected:
	std::shared_ptr<Material> m_material = nullptr;
};
