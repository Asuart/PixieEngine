#include "pch.h"
#include "VRViewportSettingsWindow.h"
#include "VRViewportWindow.h"
#include "PixieEngineApp.h"
#include "PixieEngineInterface.h"

VRViewportSettingsWindow::VRViewportSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter) {}

void VRViewportSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("VR Settings", 0)) {
		std::vector<VRViewportWindow*> viewports = m_interface.GetWindowsOfType<VRViewportWindow>();
		if (viewports.size() > 0) {
			VRViewportWindow* viewport = viewports[0];
			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();
			float fovy = viewport->m_viewportCamera.GetFieldOfViewY();
			if (ImGui::DragFloat("FoV y", &fovy, 0.01f, 0.0f, 720.0f)) {
				viewport->m_viewportCamera.SetFieldOfViewY(fovy);
			}
			ImGui::DragFloat("Distance", &viewport->m_distance, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Distortion", &viewport->m_k, 0.01f, -10.0f, 10.0f);
		}
	}
	ImGui::End();
}
