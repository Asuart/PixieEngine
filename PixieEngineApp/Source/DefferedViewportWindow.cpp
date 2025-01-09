#include "pch.h"
#include "DefferedViewportWindow.h"
#include "PixieEngineApp.h"

DefferedViewportWindow::DefferedViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter),
	m_viewportResolution({ 1280, 720 }),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 10),
	m_cameraController(m_viewportCamera) {}

void DefferedViewportWindow::Initialize() {

}

void DefferedViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Deffered Viewport##") + id.ToString()).c_str())) {
		if (ImGui::IsWindowFocused()) {
			m_cameraController.Update();
		}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		if (glmViewportResolution != m_viewportResolution) {
			UpdateViewportResolution(glmViewportResolution);
		}

		Scene* scene = m_app.GetScene();
		if (scene) {
			if (m_useCameraResolution) {

			}
			else {
				Camera camera = m_viewportCamera;
				camera.SetResolution(m_viewportResolution);
				m_app.GetDefferedRenderer()->DrawFrame(scene, &camera);
				m_app.RestoreViewportSize();
				ImGui::Image((void*)(uint64_t)m_app.GetDefferedRenderer()->m_viewportFrameBuffer.m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void DefferedViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
}
