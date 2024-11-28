#include "pch.h"
#include "MaterialsBrowserWindow.h"
#include "PixieEngineApp.h"

MaterialsBrowserWindow::MaterialsBrowserWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter) {}

void MaterialsBrowserWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Materials", 0)) {
		std::vector<Material>& materials = ResourceManager::GetMaterials();
		for (Material& material : materials) {
			ComponentRenderer::DrawMaterial(&material);
		}
	}
	ImGui::End();
}
