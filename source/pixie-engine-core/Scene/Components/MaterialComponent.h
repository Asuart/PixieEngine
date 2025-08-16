#pragma once
#include "Resources/Material.h"
#include "Component.h"

namespace PixieEngine {

class MaterialComponent : public Component {
public:
	MaterialComponent(SceneObject* parent, std::shared_ptr<Material> material);

	std::shared_ptr<Material> GetMaterial() const;
	void SetMaterial(std::shared_ptr<Material> material);

protected:
	std::shared_ptr<Material> m_material = nullptr;
};

}