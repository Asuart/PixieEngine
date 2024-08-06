#include "MaterialComponent.h"

MaterialComponent::MaterialComponent(Material* material, const SceneObject* parent)
	: Component("Material Component", parent), material(material) {}

const Material* MaterialComponent::GetMaterial() const {
	return material;
}