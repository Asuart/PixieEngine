#include "pch.h"
#include "InspectorWindow.h"
#include "PixieEngineEditor.h"
#include "SceneManager.h"

InspectorWindow::InspectorWindow(PixieEngineEditor& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void InspectorWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Inspector", 0)) {
		SceneObject* object = SceneManager::GetSelectedObject();
		if (object) {
			ImGui::Text(object->GetName().c_str());
			ImGui::Spacing();

			ComponentRenderer::DrawTransform(object->GetTransform());
			ImGui::Spacing();

			const std::vector<Component*>& components = object->GetComponents();
			if (components.size() > 0) {
				ImGui::Text("Components");
				ImGui::Spacing();
				for (size_t i = 0; i < components.size(); i++) {
					ComponentRenderer::DrawComponent(components[i]);
					ImGui::Spacing();
				}
			}
			ImGui::Spacing();
		}
	}
	ImGui::End();
}
