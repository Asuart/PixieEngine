#include "pch.h"
#include "ShaderGraphWindow.h"
#include "Interface.h"
#include "ViewportWindow.h"

void ShaderNodeConnectionObject::UpdateMesh() {
	if (!from || !to) return;
	Vec2 p0 = from->parent->position + from->offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = to->parent->position + to->offset + Vec2(4, 4);
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}

void ShaderNodeConnectionObject::UpdateTempMesh(Vec2 end) {
	Vec2 p0 = from->parent->position + from->offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = end;
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}


void ShaderGraphWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("ShaderGraph##") + id.ToString()).c_str())) {
		if (ImGui::IsWindowFocused()) {
			HandleUserInput();
		}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		ImVec2 mousePos = ImGui::GetMousePos();
		m_cursorViewportPosition = { mousePos.x - cursorScreenPos.x, mousePos.y - cursorScreenPos.y };
		m_prevCursorPos = m_cursorPos;
		m_cursorPos = m_cursorViewportPosition * m_viewScale + m_viewOffset;
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		m_frameBuffer.Resize(glmViewportResolution);
		UpdateProjection();

		GLint originalViewport[4];
		glGetIntegerv(GL_VIEWPORT, originalViewport);

		m_shaderGraph.Process();

		m_frameBuffer.Resize(glmViewportResolution);
		m_frameBuffer.Bind();
		m_frameBuffer.ResizeViewport();
		m_frameBuffer.Clear();


		GLuint id = m_shaderGraph.m_nodes.back()->GetInputTexture("Frame");
		GlobalRenderer::DrawTextureFitted(id, { 1280, 720 }, glmViewportResolution);
		glClear(GL_DEPTH_BUFFER_BIT);

		DrawConnections();
		DrawNodes();

		m_frameBuffer.Unbind();

		glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);

		ImGui::Image((void*)(uint64_t)m_frameBuffer.m_texture, viewportResolution, { 0.0f, 1.0f }, { 1.0f, 0.0f });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}