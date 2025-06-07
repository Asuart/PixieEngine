#include "pch.h"
#include "ComponentRenderer.h"
#include "PixieEngineApp.h"

void ComponentRenderer::DrawTransform(Transform& transform, bool showHead, bool defaultOpen) {
	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader("Transform", flags)) {
		const float minPosition = (float)-10000, maxPosition = (float)10000;
		const float minRotation = (float)-720, maxRotation = (float)720;
		const float minScale = (float)-10000, maxScale = (float)10000;
		glm::vec3 position = transform.GetPosition();
		if (ImGui::DragScalarN("Position", ImGuiDataType_::ImGuiDataType_Float, &position, 3, 0.01f, &minPosition, &maxPosition)) {
			transform.SetPosition(position);
		}
		glm::vec3 rotation = transform.GetEulerRotation();
		if (ImGui::DragScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Float, &rotation, 3, 0.1f, &minRotation, &maxRotation)) {
			transform.SetEulerRotation(rotation);
		}
		glm::vec3 scale = transform.GetScale();
		if (ImGui::DragScalarN("Scale", ImGuiDataType_::ImGuiDataType_Float, &scale, 3, 0.01f, &minScale, &maxScale)) {
			transform.SetScale(scale);
		}
	}
}

void ComponentRenderer::DrawMaterial(std::shared_ptr<Material> material, bool showHead, bool defaultOpen) {
	const float minZero = 0.0f;
	const float maxOne = 1.0f;
	const float maxTen = 10.0f;
	const float maxHundred = 100.0f;

	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(material->m_name.c_str(), flags)) {
		glm::vec3 albedoEditBuffer = material->m_albedo;
		if (ImGui::ColorEdit3("Albedo", &albedoEditBuffer[0])) {
			material->m_albedo = albedoEditBuffer;
		}
		glm::vec3 emissionColorEditBuffer = material->m_emissionColor;
		if (ImGui::ColorEdit3("Emission Color", &emissionColorEditBuffer[0])) {
			material->m_emissionColor = emissionColorEditBuffer;
		}
		ImGui::DragScalar("Emission Strength", ImGuiDataType_::ImGuiDataType_Float, &material->m_emissionStrength, 0.01f, &minZero, &maxHundred);
		ImGui::DragScalar("Transparency", ImGuiDataType_::ImGuiDataType_Float, &material->m_transparency, 0.01f, &minZero, &maxOne);
		ImGui::DragScalar("Refraction", ImGuiDataType_::ImGuiDataType_Float, &material->m_refraction, 0.01f, &minZero, &maxTen);
		ImGui::DragScalar("Roughness", ImGuiDataType_::ImGuiDataType_Float, &material->m_roughness, 0.01f, &minZero, &maxOne);
		ImGui::DragScalar("Metallic", ImGuiDataType_::ImGuiDataType_Float, &material->m_metallic, 0.01f, &minZero, &maxOne);
	}
}

void ComponentRenderer::DrawComponent(Component* component, bool showHead, bool defaultOpen) {
	const float minZero = 0.0f;
	const float maxOne = 1.0f;
	const float maxTen = 10.0f;
	const float maxNinety = 90.0f;

	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(typeid(*component).name(), flags)) {
		uint64_t type = typeid(*component).hash_code();
		if (type == typeid(MaterialComponent).hash_code()) {
			MaterialComponent* materialComponent = dynamic_cast<MaterialComponent*>(component);
			ImGui::Text((std::string("Material name: ") + materialComponent->GetMaterial()->m_name).c_str());
			ComponentRenderer::DrawMaterial(materialComponent->GetMaterial(), false);
		}
		else if (type == typeid(MeshComponent).hash_code()) {
			MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);
			ImGui::Text((std::string("Vertices count: ") + std::to_string(meshComponent->GetMesh().GetIndicesCount())).c_str());
			ImGui::Text((std::string("Triangles count: ") + std::to_string(meshComponent->GetMesh().GetIndicesCount() / 3)).c_str());
		}
		else if (type == typeid(CameraComponent).hash_code()) {
			CameraComponent* cameraComponent = dynamic_cast<CameraComponent*>(component);
			Camera& camera = cameraComponent->GetCamera();
			float fovy = camera.GetFieldOfViewY();
			if (ImGui::DragScalar("Field of Voew Y", ImGuiDataType_::ImGuiDataType_Float, &fovy, 0.01f, &minZero, &maxNinety)) {
				camera.SetFieldOfViewY(fovy);
			}
		}
		else if (type == typeid(PointLightComponent).hash_code()) {
			PointLightComponent* plComponent = dynamic_cast<PointLightComponent*>(component);
			glm::vec3 color = plComponent->GetColor();
			if (ImGui::ColorEdit3("Emission Color", &color[0])) {
				plComponent->SetColor(color);
			}
			float strength = plComponent->GetStrength();
			if (ImGui::DragScalar("Emission Strength", ImGuiDataType_::ImGuiDataType_Float, &strength, 0.01f, &minZero, &maxNinety)) {
				plComponent->SetStrength(strength);
			}
		}
		else if (type == typeid(DirectionalLightComponent).hash_code()) {
			DirectionalLightComponent* dlComponent = dynamic_cast<DirectionalLightComponent*>(component);
			glm::vec3 color = dlComponent->GetColor();
			if (ImGui::ColorEdit3("Emission Color", &color[0])) {
				dlComponent->SetColor(color);
			}
			float strength = dlComponent->GetStrength();
			if (ImGui::DragScalar("Emission Strength", ImGuiDataType_::ImGuiDataType_Float, &strength, 0.01f, &minZero, &maxNinety)) {
				dlComponent->SetStrength(strength);
			}
		}
		else if (type == typeid(SphereComponent).hash_code()) {
			SphereComponent* sphereComponent = dynamic_cast<SphereComponent*>(component);
			float radius = sphereComponent->GetRadius();
			if (ImGui::DragScalar("Radius", ImGuiDataType_::ImGuiDataType_Float, &radius, 0.01f, &minZero, &maxNinety)) {
				sphereComponent->SetRadius(radius);
			}
		}
	}
}
