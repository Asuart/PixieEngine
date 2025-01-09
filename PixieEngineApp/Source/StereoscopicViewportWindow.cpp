#include "pch.h"
#include "StereoscopicViewportWindow.h"
#include "PixieEngineApp.h"

StereoscopicViewportWindow::StereoscopicViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter),
	m_viewportResolution(glm::ivec2(1280, 720)),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 10),
	m_cameraController(m_viewportCamera) {}

void StereoscopicViewportWindow::Initialize() {
	m_viewportFrameBuffer = new FrameBuffer({ 1280, 720 });
	m_viewportFrameBufferLeft = new FrameBuffer({ 1280, 720 });
	m_viewportFrameBufferRight = new FrameBuffer({ 1280, 720 });
	m_stereoscopicShader = ResourceManager::LoadShader("StereoscopicQuadVertexShader.glsl", "StereoscopicQuadFragmentShader.glsl");
}

void StereoscopicViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Stereoscopic Viewport##") + id.ToString()).c_str())) {
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
			Camera c = m_viewportCamera;

			c.GetTransform().Translate(c.GetTransform().GetRight() * -m_distance / 2.0f);
			m_viewportFrameBufferLeft->Bind();
			glViewport(0, 0, m_viewportFrameBufferLeft->m_resolution.x, m_viewportFrameBufferLeft->m_resolution.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_app.GetDefaultRenderer()->DrawFrame(scene, &c);
			m_viewportFrameBufferLeft->Unbind();

			c.GetTransform().Translate(c.GetTransform().GetRight() * m_distance);
			m_viewportFrameBufferRight->Bind();
			glViewport(0, 0, m_viewportFrameBufferRight->m_resolution.x, m_viewportFrameBufferRight->m_resolution.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_app.GetDefaultRenderer()->DrawFrame(scene, &c);
			m_viewportFrameBufferRight->Unbind();

			m_viewportFrameBuffer->Bind();
			glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_stereoscopicShader.Bind();

			Vec2 pos(0.0f, 0.0f), size(1.0f, 1.0f);
			Float textureAspect = Aspect(m_viewportResolution);
			Float viewportAspect = Aspect(m_viewportResolution);
			if (viewportAspect > textureAspect) {
				size.x = textureAspect / viewportAspect;
				pos.x = (1.0f - size.x) * 0.5f;
			}
			else {
				size.y = viewportAspect / textureAspect;
				pos.y = (1.0f - size.y) * 0.5f;
			}

			m_stereoscopicShader.SetUniform2f("uPos", pos);
			m_stereoscopicShader.SetUniform2f("uSize", size);

			m_stereoscopicShader.SetTexture("textureLeft", m_viewportFrameBufferLeft->m_texture, 0);
			m_stereoscopicShader.SetTexture("textureRight", m_viewportFrameBufferRight->m_texture, 1);

			m_stereoscopicShader.SetUniform3f("uBalance", { m_redBalance, m_greenBalance, m_blueBalance });
			m_stereoscopicShader.SetUniform3f("uScale", { m_redScale, m_greenScale, m_blueScale });

			ResourceManager::GetQuadMesh()->Draw();

			m_viewportFrameBuffer->Unbind();
			m_app.RestoreViewportSize();
		}

		ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void StereoscopicViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_viewportFrameBuffer->Resize(resolution);
	m_viewportFrameBufferLeft->Resize(resolution);
	m_viewportFrameBufferRight->Resize(resolution);
}
