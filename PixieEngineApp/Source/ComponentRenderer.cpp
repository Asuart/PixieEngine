#include "pch.h"
#include "ComponentRenderer.h"
#include "PixieEngineApp.h"

PixieEngineApp* ComponentRenderer::m_app = nullptr;

void ComponentRenderer::Initialize(PixieEngineApp* app) {
	m_app = app;
}

void ComponentRenderer::DrawTransform(Transform& transform, bool showHead, bool defaultOpen) {
	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if(!showHead || ImGui::CollapsingHeader("Transform", flags)) {
		const Float minPosition = (Float)-10000, maxPosition = (Float)10000;
		const Float minRotation = (Float)-720, maxRotation = (Float)720;
		const Float minScale = (Float)-10000, maxScale = (Float)10000;
		Vec3 position = transform.GetPosition();
		if (ImGui::DragScalarN("Position", ImGuiFloat, &position, 3, 0.01f, &minPosition, &maxPosition)) {
			transform.SetPosition(position);
		}
		Vec3 rotation = transform.GetEulerRotation();
		if (ImGui::DragScalarN("Rotation", ImGuiFloat, &rotation, 3, 0.1f, &minRotation, &maxRotation)) {
			transform.SetEulerRotation(rotation);
		}
		Vec3 scale = transform.GetScale();
		if (ImGui::DragScalarN("Scale", ImGuiFloat, &scale, 3, 0.01f, &minScale, &maxScale)) {
			transform.SetScale(scale);
		}
	}
}

void ComponentRenderer::DrawMaterial(Material* material, bool showHead, bool defaultOpen) {
	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(material->m_name.c_str(), flags)) {
		const Float minZero = (Float)0;
		const Float maxOne = (Float)1;
		const Float maxTen = (Float)10;

		glm::fvec3 albedoEditBuffer = material->m_albedo.GetRGB();
		if (ImGui::ColorEdit3("Albedo", &albedoEditBuffer[0])) {
			material->m_albedo.SetRGB((Vec3)albedoEditBuffer);
			m_app->ResetRayTracers();
		}
		glm::fvec3 emissionColorEditBuffer = material->m_emissionColor.GetRGB();
		if (ImGui::ColorEdit3("Emission Color", &emissionColorEditBuffer[0])) {
			material->m_emissionColor.SetRGB((Vec3)emissionColorEditBuffer);
			m_app->ResetRayTracers();
		}
		if (ImGui::DragScalar("Emission Strength", ImGuiFloat, &material->m_emissionStrength)) {
			m_app->ResetRayTracers();
		}
		if (ImGui::DragScalar("Roughness", ImGuiFloat, &material->m_roughness, 0.01f, &minZero, &maxOne)) {
			m_app->ResetRayTracers();
		}
		if (ImGui::DragScalar("Metallic", ImGuiFloat, &material->m_metallic, 0.01f, &minZero, &maxOne)) {
			m_app->ResetRayTracers();
		}
		if (ImGui::DragScalar("Transparency", ImGuiFloat, &material->m_transparency, 0.01f, &minZero, &maxOne)) {
			m_app->ResetRayTracers();
		}
		if (ImGui::DragScalar("Refraction", ImGuiFloat, &material->m_refraction, 0.01f, &minZero, &maxTen)) {
			m_app->ResetRayTracers();
		}
	}
}

void ComponentRenderer::DrawComponent(Component* component, bool showHead, bool defaultOpen) {
	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(to_string(component->type).c_str(), flags)) {
		if (component->type == ComponentType::Material) {
			MaterialComponent* materialComponent = dynamic_cast<MaterialComponent*>(component);
			ImGui::Text((std::string("Material name: ") + materialComponent->GetMaterial()->m_name).c_str());
			ComponentRenderer::DrawMaterial(materialComponent->GetMaterial(), false);
		}
	}
}
