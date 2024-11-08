#include "pch.h"
#include "VRViewportWindow.h"
#include "PixieEngineApp.h"

VRViewportWindow::VRViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter),
	m_viewportResolution(glm::ivec2(1280, 720)),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), 8.0f / 9.0f, 0, 10),
	m_cameraController(m_viewportCamera) {}

void VRViewportWindow::Initialize() {
	m_viewportFrameBuffer = new FrameBuffer(1280, 720);
	m_viewportFrameBufferLeft = new FrameBuffer(640, 720);
	m_viewportFrameBufferRight = new FrameBuffer(640, 720);
	m_vrShader = CompileShader(VR_QUAD_VERTEX_SHADER_SOURCE, VR_QUAD_FRAGMENT_SHADER_SOURCE);
}

void VRViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("VR Viewport##") + id.ToString()).c_str())) {
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
			glUseProgram(m_vrShader);

			GLuint posLoc = glGetUniformLocation(m_vrShader, "uPos");
			GLuint sizeLoc = glGetUniformLocation(m_vrShader, "uSize");
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

			GLuint textureLeftLoc = glGetUniformLocation(m_vrShader, "textureLeft");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_viewportFrameBufferLeft->m_texture);
			glUniform1i(textureLeftLoc, 0);

			GLuint textureRightLoc = glGetUniformLocation(m_vrShader, "textureRight");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_viewportFrameBufferRight->m_texture);
			glUniform1i(textureRightLoc, 1);

			GLuint balanceLoc = glGetUniformLocation(m_vrShader, "uDistance");
			glUniform1f(balanceLoc, m_distance);
			GLuint scaleLoc = glGetUniformLocation(m_vrShader, "uK");
			glUniform1f(scaleLoc, m_k);

			ResourceManager::GetQuadMesh()->Draw();

			m_viewportFrameBuffer->Unbind();
			m_app.RestoreViewportSize();
		}

		ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void VRViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	glm::ivec2 hwResolution = glm::ivec2(resolution.x / 2, resolution.y);
	m_viewportResolution = resolution;
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
	m_viewportFrameBufferLeft->Resize(hwResolution.x, hwResolution.y);
	m_viewportFrameBufferRight->Resize(hwResolution.x, hwResolution.y);
	m_viewportCamera.SetAspect((float)hwResolution.x / hwResolution.y);
}
