#include "pch.h"
#include "ComponentRenderer.h"
#include "PixieEngineApp.h"

void ComponentRenderer::DrawTransform(Transform& transform, bool showHead, bool defaultOpen) {
	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if(!showHead || ImGui::CollapsingHeader("Transform", flags)) {
		const Float minPosition = (Float)-10000, maxPosition = (Float)10000;
		const Float minRotation = (Float)-720, maxRotation = (Float)720;
		const Float minScale = (Float)-10000, maxScale = (Float)10000;
		Vec3 position = transform.GetPosition();
		if (ImGui::DragScalarN("Position", ImGuiFloat, &position, 3, 0.01f, &minPosition, &maxPosition)) {
			transform.SetPosition(position);
			SceneManager::UpdateSceneSnapshot();
		}
		Vec3 rotation = transform.GetEulerRotation();
		if (ImGui::DragScalarN("Rotation", ImGuiFloat, &rotation, 3, 0.1f, &minRotation, &maxRotation)) {
			transform.SetEulerRotation(rotation);
			SceneManager::UpdateSceneSnapshot();
		}
		Vec3 scale = transform.GetScale();
		if (ImGui::DragScalarN("Scale", ImGuiFloat, &scale, 3, 0.01f, &minScale, &maxScale)) {
			transform.SetScale(scale);
			SceneManager::UpdateSceneSnapshot();
		}
	}
}

void ComponentRenderer::DrawMaterial(Material* material, bool showHead, bool defaultOpen) {
	const Float minZero = 0.0f;
	const Float maxOne = 1.0f;
	const Float maxTen = 10.0f;

	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(material->m_name.c_str(), flags)) {
		glm::fvec3 albedoEditBuffer = material->m_albedo.GetRGB();
		if (ImGui::ColorEdit3("Albedo", &albedoEditBuffer[0])) {
			material->m_albedo.SetRGB((Vec3)albedoEditBuffer);
			SceneManager::UpdateSceneSnapshot();
		}
		glm::fvec3 emissionColorEditBuffer = material->m_emissionColor.GetRGB();
		if (ImGui::ColorEdit3("Emission Color", &emissionColorEditBuffer[0])) {
			material->m_emissionColor.SetRGB((Vec3)emissionColorEditBuffer);
			SceneManager::UpdateSceneSnapshot();
		}
		if (ImGui::DragScalar("Emission Strength", ImGuiFloat, &material->m_emissionStrength)) {
			SceneManager::UpdateSceneSnapshot();
		}
		if (ImGui::DragScalar("Roughness", ImGuiFloat, &material->m_roughness, 0.01f, &minZero, &maxOne)) {
			SceneManager::UpdateSceneSnapshot();
		}
		if (ImGui::DragScalar("Metallic", ImGuiFloat, &material->m_metallic, 0.01f, &minZero, &maxOne)) {
			SceneManager::UpdateSceneSnapshot();
		}
		if (ImGui::DragScalar("Transparency", ImGuiFloat, &material->m_transparency, 0.01f, &minZero, &maxOne)) {
			SceneManager::UpdateSceneSnapshot();
		}
		if (ImGui::DragScalar("Refraction", ImGuiFloat, &material->m_refraction, 0.01f, &minZero, &maxTen)) {
			SceneManager::UpdateSceneSnapshot();
		}
	}
}

void ComponentRenderer::DrawComponent(Component* component, bool showHead, bool defaultOpen) {
	const Float minZero = 0.0f;
	const Float maxOne = 1.0f;
	const Float maxTen = 10.0f;
	const Float maxNinety = 90.0f;

	ImGuiTreeNodeFlags flags = defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
	if (!showHead || ImGui::CollapsingHeader(to_string(component->type).c_str(), flags)) {
		switch (component->type) {
		case ComponentType::Material: {
			MaterialComponent* materialComponent = dynamic_cast<MaterialComponent*>(component);
			ImGui::Text((std::string("Material name: ") + materialComponent->GetMaterial()->m_name).c_str());
			ComponentRenderer::DrawMaterial(materialComponent->GetMaterial(), false);
			break;
		}
		case ComponentType::Mesh: {
			MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);
			ImGui::Text((std::string("Vertices count: ") + std::to_string(meshComponent->GetMesh()->m_vertices.size())).c_str());
			ImGui::Text((std::string("Triangles count: ") + std::to_string(meshComponent->GetMesh()->m_indices.size() / 3)).c_str());
			break;
		}
		case ComponentType::Camera: {
			CameraComponent* cameraComponent = dynamic_cast<CameraComponent*>(component);
			Camera& camera = cameraComponent->GetCamera();
			Float fovy = camera.GetFieldOfViewY();
			if (ImGui::DragScalar("Field of Voew Y", ImGuiFloat, &fovy, 0.01f, &minZero, &maxNinety)) {
				camera.SetFieldOfViewY(fovy);
			}
			break;
		}
		case ComponentType::PointLight: {
			PointLightComponent* plComponent = dynamic_cast<PointLightComponent*>(component);
			Vec3 color = plComponent->GetColor();
			if (ImGui::ColorEdit3("Emission Color", &color[0])) {
				plComponent->SetColor(color);
				SceneManager::UpdateSceneSnapshot();
			}
			Float strength = plComponent->GetStrength();
			if (ImGui::DragScalar("Emission Strength", ImGuiFloat, &strength, 0.01f, &minZero, &maxNinety)) {
				plComponent->SetStrength(strength);
				SceneManager::UpdateSceneSnapshot();
			}
			break;
		}
		case ComponentType::DirectionalLight: {
			DirectionalLightComponent* dlComponent = dynamic_cast<DirectionalLightComponent*>(component);
			Vec3 color = dlComponent->GetColor();
			if (ImGui::ColorEdit3("Emission Color", &color[0])) {
				dlComponent->SetColor(color);
				SceneManager::UpdateSceneSnapshot();
			}
			Float strength = dlComponent->GetStrength();
			if (ImGui::DragScalar("Emission Strength", ImGuiFloat, &strength, 0.01f, &minZero, &maxNinety)) {
				dlComponent->SetStrength(strength);
				SceneManager::UpdateSceneSnapshot();
			}
			break;
		}
		case ComponentType::Sphere: {
			SphereComponent* sphereComponent = dynamic_cast<SphereComponent*>(component);
			Float radius = sphereComponent->GetRadius();
			if (ImGui::DragScalar("Radius", ImGuiFloat, &radius, 0.01f, &minZero, &maxNinety)) {
				sphereComponent->SetRadius(radius);
				SceneManager::UpdateSceneSnapshot();
			}
			break;
		}
		}
	}
}
