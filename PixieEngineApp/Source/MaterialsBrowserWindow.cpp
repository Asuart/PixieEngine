#include "pch.h"
#include "MaterialsBrowserWindow.h"
#include "PixieEngineApp.h"

MaterialsBrowserWindow::MaterialsBrowserWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter) {}

void MaterialsBrowserWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Materials", 0)) {
		std::map<std::string, Material*>& materials = ResourceManager::GetMaterials();
		for (std::pair<std::string, Material*> entry : materials) {
			Material* material = entry.second;
			ComponentRenderer::DrawMaterial(material);
		}
	}
	ImGui::End();
}
