#include "pch.h"
#include "ViewportWindow.h"
#include "SceneManager.h"

std::string to_string(RenderMode renderMode) {
	switch (renderMode) {
	case RenderMode::Forward: return "Forward";
	case RenderMode::Deffered:	return "Deffered";
	default: return "Wrong Render Mode";
	}
}

ViewportWindow::ViewportWindow(PixieEngineEditor& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1280, 720 }), m_cameraController(m_viewportCamera),
	m_viewportCamera(glm::vec3(-10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 0.01f, 100.0f) {
	ResetRenderer();
}

void ViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Viewport##")).c_str())) {
		if (ImGui::IsWindowFocused()) {
			if (m_cameraController.Update()) {}
		}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		m_frameBuffer.Resize(glmViewportResolution);

		std::shared_ptr<Scene> scene = SceneManager::GetScene();
		if (scene) {
			GLint originalViewport[4];
			glGetIntegerv(GL_VIEWPORT, originalViewport);

			HighPrecisionTimer::StartTimer("Viewport Render Time");
			m_renderer->DrawFrame(scene, m_viewportCamera);

			m_frameBuffer.Bind();
			m_frameBuffer.ResizeViewport();
			m_frameBuffer.Clear();
			RenderEngine::DrawTextureFitted(m_renderer->GetFrameHandle(), m_renderer->GetResolution(), glmViewportResolution);
			m_frameBuffer.Unbind();
			HighPrecisionTimer::StopTimer("Viewport Render Time");

			glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);

			ImGui::Image((void*)(uint64_t)m_frameBuffer.GetColorHandle(), viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

RenderMode ViewportWindow::GetRenderMode() const {
	return m_renderMode;
}

void ViewportWindow::SetRenderMode(RenderMode renderMode) {
	m_renderMode = renderMode;
	ResetRenderer();
}

AntiAliasing ViewportWindow::GetAntiAliasing() const {
	return m_renderer->GetAntiAliasing();
}

void ViewportWindow::SetAntiAliasing(AntiAliasing mode) {
	m_renderer->SetAntiAliasing(mode);
}

const FrameBuffer& ViewportWindow::GetFrameBuffer() {
	return m_frameBuffer;
}

void ViewportWindow::ResetRenderer() {
	if (m_renderer) {
		delete m_renderer;
	}
	switch (m_renderMode) {
	case RenderMode::Forward:
		m_renderer = new ForwardRenderer();
		break;
	case RenderMode::Deffered:
		m_renderer = new DefferedRenderer();
		break;
	default: 
		m_renderer = new ForwardRenderer();
		m_renderMode = RenderMode::Forward;
		break;
	}
	m_renderer->SetResolution(m_frameBuffer.GetResolution());
}

