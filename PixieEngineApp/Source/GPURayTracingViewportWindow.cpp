#include "pch.h"
#include "GPURayTracingViewportWindow.h"
#include "PixieEngineApp.h"

AlignedTriangle::AlignedTriangle(Triangle tri) :
	p0(tri.p0), p1(tri.p1), p2(tri.p2), material(tri.m_materialIndex), area(0) {}

AlignedMaterial::AlignedMaterial(Material& material) :
	albedo(Vec4(material.m_albedo.GetRGB(), 1.0f)), emission(Vec4(material.GetEmission().GetRGB(), 1.0f)) {}

AlignedNode::AlignedNode(BVHNode& node) :
	pMin(Vec4(node.pMin, 0.0f)), pMax(Vec4(node.pMax, 0.0f)),
	nPrimitives(node.nTriangles), secondChildOffset(node.childOffset), axis(node.axis) {}

GPURayTracingViewportWindow::GPURayTracingViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 10),
	m_cameraController(m_viewportCamera) {}

void GPURayTracingViewportWindow::Initialize() {
	m_viewportFrameBuffer = new FrameBuffer({ 1280, 720 });

	m_shader = ResourceManager::LoadComputeShader("PathTracingComputeShader.glsl");

	glGenTextures(1, &m_resultTexture);
	glBindTexture(GL_TEXTURE_2D, m_resultTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureStorage2D(m_resultTexture, 1, GL_RGBA32F, m_resultTextureResolution.x, m_resultTextureResolution.y);
	glBindImageTexture(0, m_resultTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void GPURayTracingViewportWindow::Draw() {
	if (m_sceneSnapshot != m_app.GetSceneSnapshot()) {
		UpdateScene();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("GPU Ray Tracing Viewport")) {
		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		if (glmViewportResolution != m_viewportResolution) {
			UpdateViewportResolution(glmViewportResolution);
		}

		if (ImGui::IsWindowFocused()) {
			if (m_cameraController.Update()) {
				m_samples = 0;
			}
		}

		if (m_numTriangles > 0) {
			m_shader.Bind();
			m_shader.SetUniform1i("uNumTriangles", m_numTriangles);
			m_shader.SetUniform1i("uTime", (GLuint)Time::lastTime.count());
			m_shader.SetUniform1i("uReset", m_samples == 0);
			m_shader.SetUniformMat4f("uViewMatrix", m_viewportCamera.GetInverseViewMatrix());
			glDispatchCompute((GLuint)m_resultTextureResolution.x / 8, (GLuint)m_resultTextureResolution.y / 4, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			m_samples++;
		}

		m_viewportFrameBuffer->Bind();
		glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_app.GetDefaultRenderer()->DrawTexture(m_resultTexture, m_resultTextureResolution, m_viewportResolution, m_samples);
		m_viewportFrameBuffer->Unbind();
		m_app.RestoreViewportSize();

		ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void GPURayTracingViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_viewportFrameBuffer->Resize(resolution);
}

void GPURayTracingViewportWindow::UpdateScene() {
	m_sceneSnapshot = m_app.GetSceneSnapshot();
	if (!m_sceneSnapshot) {
		return;
	}

	std::vector<Triangle>& triangles = m_sceneSnapshot->GetTriangles();
	std::vector<AlignedTriangle> alignedTriangles;
	alignedTriangles.resize(triangles.size());
	for (size_t i = 0; i < triangles.size(); i++) {
		alignedTriangles[i] = AlignedTriangle(triangles[i]);
	}
	m_numTriangles = (int32_t)alignedTriangles.size();
	int32_t trianglesBufferSize = m_numTriangles * sizeof(alignedTriangles[0]);

	if (!m_trianglesBuffer) {
		glGenBuffers(1, &m_trianglesBuffer);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_trianglesBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_trianglesBuffer, 0, trianglesBufferSize);
	glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesBufferSize, alignedTriangles.data(), GL_STATIC_READ);

	std::vector<Material>& materials = ResourceManager::GetMaterials();
	std::vector<AlignedMaterial> alignedMaterials;
	alignedMaterials.resize(materials.size());
	for (size_t i = 0; i < materials.size(); i++) {
		alignedMaterials[i] = AlignedMaterial(materials[i]);
	}
	int32_t materialsBufferSize = (int32_t)materials.size() * sizeof(AlignedMaterial);

	if (!m_materialsBuffer) {
		glGenBuffers(1, &m_materialsBuffer);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialsBuffer);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_materialsBuffer, 0, materialsBufferSize);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materialsBufferSize, alignedMaterials.data(), GL_STATIC_READ);

	m_samples = 0;
}
