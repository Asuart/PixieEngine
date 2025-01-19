#include "pch.h"
#include "MaterialComponent.h"

MaterialComponent::MaterialComponent(SceneObject* parent, Material* material) :
	Component(ComponentType::Material, parent), m_material(material) {}

Material* MaterialComponent::GetMaterial() {
	return m_material;
}

void MaterialComponent::SetMaterial(Material* _material) {
	m_material = _material;
}
