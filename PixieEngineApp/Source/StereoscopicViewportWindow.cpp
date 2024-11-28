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
	m_stereoscopicShader = ResourceManager::CompileShader(STEREOSCOPIC_QUAD_VERTEX_SHADER_SOURCE, STEREOSCOPIC_QUAD_FRAGMENT_SHADER_SOURCE);
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
			glUseProgram(m_stereoscopicShader);

			GLuint posLoc = glGetUniformLocation(m_stereoscopicShader, "uPos");
			GLuint sizeLoc = glGetUniformLocation(m_stereoscopicShader, "uSize");
			float textureAspect = (float)m_viewportResolution.x / m_viewportResolution.y;
			float viewportAspect = (float)m_viewportResolution.x / m_viewportResolution.y;
			float posX, posY;
			float sizeX, sizeY;
			if (viewportAspect > textureAspect) {
				sizeY = 1.0f;
				sizeX = textureAspect / viewportAspect;
				posX = (1.0f - sizeX) * 0.5f;
				posY = 0.0f;
			}
			else {
				sizeX = 1.0f;
				sizeY = viewportAspect / textureAspect;
				posX = 0.0f;
				posY = (1.0f - sizeY) * 0.5f;
			}

			glUniform2f(posLoc, posX, posY);
			glUniform2f(sizeLoc, sizeX, sizeY);

			GLuint textureLeftLoc = glGetUniformLocation(m_stereoscopicShader, "textureLeft");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_viewportFrameBufferLeft->m_texture);
			glUniform1i(textureLeftLoc, 0);

			GLuint textureRightLoc = glGetUniformLocation(m_stereoscopicShader, "textureRight");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_viewportFrameBufferRight->m_texture);
			glUniform1i(textureRightLoc, 1);

			GLuint balanceLoc = glGetUniformLocation(m_stereoscopicShader, "uBalance");
			glUniform3f(balanceLoc, m_redBalance, m_greenBalance, m_blueBalance);
			GLuint scaleLoc = glGetUniformLocation(m_stereoscopicShader, "uScale");
			glUniform3f(scaleLoc, m_redScale, m_greenScale, m_blueScale);

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
