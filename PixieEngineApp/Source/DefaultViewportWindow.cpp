#include "pch.h"
#include "DefaultViewportWindow.h"
#include "PixieEngineApp.h"

DefaultViewportWindow::DefaultViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter),
	m_viewportResolution(glm::ivec2(1280, 720)),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 0, 10),
	m_cameraController(m_viewportCamera) {}

void DefaultViewportWindow::Initialize() {
	m_viewportFrameBuffer = new FrameBuffer(1280, 720);
}

void DefaultViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Default Viewport##") + id.ToString()).c_str())) {
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
			m_viewportFrameBuffer->Bind();
			glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_app.GetDefaultRenderer()->DrawFrame(scene, &m_viewportCamera);

			m_viewportFrameBuffer->Unbind();
			m_app.RestoreViewportSize();
		}

		ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void DefaultViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
	m_viewportCamera.SetAspect((float)resolution.x / resolution.y);
}