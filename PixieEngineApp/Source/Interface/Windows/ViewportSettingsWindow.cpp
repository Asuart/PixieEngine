#include "pch.h"
#include "ViewportSettingsWindow.h"
#include "ViewportWindow.h"
#include "PixieEngineApp.h"
#include "Interface.h"

ViewportSettingsWindow::ViewportSettingsWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void ViewportSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Viewport Settings", 0)) {
		std::vector<ViewportWindow*> viewports = m_interface.GetWindowsOfType<ViewportWindow>();
		if (viewports.size() > 0) {
			ViewportWindow* viewport = viewports[0];
			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();

			float fovy = viewport->m_viewportCamera.GetFieldOfViewY();
			if (ImGui::DragFloat("Field of View y", &fovy, 0.01f, 0.0f, 720.0f)) {
				viewport->m_viewportCamera.SetFieldOfViewY(fovy);
			}
			ImGui::Spacing();

			RenderMode activeRenderMode = viewport->GetRenderMode();
			if (ImGui::BeginCombo("Render Mode", to_string(activeRenderMode).c_str())) {
				for (int32_t n = 0; n < (int32_t)RenderMode::COUNT; n++) {
					RenderMode mode = RenderMode(n);
					bool isSelected = (activeRenderMode == mode);
					if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
						viewport->SetRenderMode(mode);
					}
					if (isSelected) {
						activeRenderMode = mode;
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

			if (activeRenderMode == RenderMode::Forward) {
				AntiAliasing activeAntiAlising = viewport->GetAntiAliasing();
				if (ImGui::BeginCombo("Anti Aliasing", to_string(activeAntiAlising).c_str())) {
					for (int32_t n = 0; n < (int32_t)AntiAliasing::COUNT; n++) {
						AntiAliasing mode = AntiAliasing(n);
						bool isSelected = (activeAntiAlising == mode);
						if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
							viewport->SetAntiAliasing(mode);
						}
						if (isSelected) {
							activeAntiAlising = mode;
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Spacing();
			}
		}
	}
	ImGui::End();
}
