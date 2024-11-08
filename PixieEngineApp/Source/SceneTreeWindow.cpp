#include "pch.h"
#include "SceneTreeWindow.h"
#include "PixieEngineApp.h"

SceneTreeWindow::SceneTreeWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter) {}

void SceneTreeWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Scene", 0)) {
		Scene* scene = m_app.GetScene();
		if (scene) {
			DrawSceneTree(scene->GetRootObject());
		}
		if (ImGui::IsWindowFocused()) {
			HandleUserInput();
		}
	}
	ImGui::End();
}

void SceneTreeWindow::DrawSceneTree(SceneObject* object) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (object == m_app.GetScene()->GetRootObject()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (object->GetChildren().size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (object == m_app.GetSelectedObject()) flags |= ImGuiTreeNodeFlags_Selected;
	bool openTreeNode = ImGui::TreeNodeEx(object->GetName().c_str(), flags);
	if (ImGui::IsItemClicked()) {
		m_app.SelectObject(const_cast<SceneObject*>(object));
	}
	if (openTreeNode) {
		for (size_t i = 0; i < object->GetChildren().size(); i++) {
			DrawSceneTree(object->GetChild(i));
		}
		ImGui::TreePop();
	}
}

void SceneTreeWindow::HandleUserInput() {
	if (UserInput::GetKey(GLFW_KEY_DELETE)) {
		m_app.RemoveSelectedObject();
	}
}
