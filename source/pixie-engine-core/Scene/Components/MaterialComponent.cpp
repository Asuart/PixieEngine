#include "pch.h"
#include "MaterialComponent.h"

namespace PixieEngine {

MaterialComponent::MaterialComponent(SceneObject* parent, std::shared_ptr<Material> material) :
	Component(ComponentType::Material, parent), m_material(material) {
}

std::shared_ptr<Material> MaterialComponent::GetMaterial() const {
	return m_material;
}

void MaterialComponent::SetMaterial(std::shared_ptr<Material> _material) {
	m_material = _material;
}

}