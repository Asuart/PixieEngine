#include "pch.h"
#include "MaterialComponent.h"

MaterialComponent::MaterialComponent(Material* material, SceneObject* parent)
	: Component(ComponentType::Material, parent), m_material(material) {}

Material* MaterialComponent::GetMaterial() {
	return m_material;
}

void MaterialComponent::SetMaterial(Material* _material) {
	m_material = _material;
}
