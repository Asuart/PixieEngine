#include "pch.h"
#include "RendererViewportSettingsWindow.h"
#include "DefaultViewportWindow.h"
#include "PixieEngineApp.h"
#include "PixieEngineInterface.h"

RendererViewportSettingsWindow::RendererViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter) {}

void RendererViewportSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Renderer Settings", 0)) {
		std::vector<DefaultViewportWindow*> viewports = m_interface.GetWindowsOfType<DefaultViewportWindow>();
		if (viewports.size() > 0) {
			DefaultViewportWindow* viewport = viewports[0];
			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();
			float fovy = viewport->m_viewportCamera.GetFieldOfViewY();
			if (ImGui::DragFloat("Field of View y", &fovy, 0.01f, 0.0f, 720.0f)) {
				viewport->m_viewportCamera.SetFieldOfViewY(fovy);
			}
			ImGui::Spacing();
			if (ImGui::Checkbox("Use Camera Resolution", &viewport->m_useCameraResolution)) {}
			if (viewport->m_useCameraResolution) {
				glm::ivec2 resolution = viewport->m_viewportCamera.GetResolution();
				if (ImGui::InputInt2("Camera Resolution", &resolution[0])) {
					resolution = glm::clamp(resolution, 1, 4096);
					viewport->m_viewportCamera.SetResolution(resolution);
				}
				ImGui::Text((std::string("Aspect: ") + std::to_string(Aspect(resolution))).c_str());
			}
		}
	}
	ImGui::End();
}
