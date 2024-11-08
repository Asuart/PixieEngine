#include "pch.h"
#include "StereoscopicViewportSettingsWindow.h"
#include "StereoscopicViewportWindow.h"
#include "PixieEngineApp.h"
#include "PixieEngineInterface.h"

StereoscopicViewportSettingsWindow::StereoscopicViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter) {}

void StereoscopicViewportSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Stereo Settings", 0)) {
		std::vector<StereoscopicViewportWindow*> viewports = m_interface.GetWindowsOfType<StereoscopicViewportWindow>();
		if (viewports.size() > 0) {
			StereoscopicViewportWindow* viewport = viewports[0];
			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();
			float fovy = viewport->m_viewportCamera.GetFieldOfView();
			if (ImGui::DragFloat("FoV y", &fovy, 0.01f, 0.0f, 720.0f)) {
				viewport->m_viewportCamera.SetFieldOfViewY(fovy);
			}
			ImGui::DragFloat("Distance", &viewport->m_distance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Red Balance", &viewport->m_redBalance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Red Scale", &viewport->m_redScale, 0.01f, 0.0f, 2.0f);
			ImGui::DragFloat("Green Balance", &viewport->m_greenBalance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Green Scale", &viewport->m_greenScale, 0.01f, 0.0f, 2.0f);
			ImGui::DragFloat("Blue Balance", &viewport->m_blueBalance, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Blue Scale", &viewport->m_blueScale, 0.01f, 0.0f, 2.0f);
		}
	}
	ImGui::End();
}
