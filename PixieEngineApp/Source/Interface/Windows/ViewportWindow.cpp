#include "pch.h"
#include "ViewportWindow.h"

struct AlignedTriangle {
	glm::fvec3 p0 = glm::fvec3(0.0f);
	int32_t material = 0;
	glm::fvec3 p1 = glm::fvec3(0.0f);
	float area = 0;
	glm::fvec3 p2 = glm::fvec3(0.0f);
	float padding = 0;

	AlignedTriangle() = default;
	AlignedTriangle(Triangle tri);
};

struct AlignedMaterial {
	Vec4 albedo;
	Vec4 emission;

	AlignedMaterial() = default;
	AlignedMaterial(Material& material);
};

struct AlignedNode {
	Vec4 pMin;
	Vec4 pMax;
	int32_t secondChildOffset;
	int16_t nPrimitives;
	int16_t axis;
	int64_t padding = 0;

	AlignedNode() = default;
	AlignedNode(BVHNode& node);
};


//Camera m_viewportCamera;
//ViewportCameraController m_cameraController;
//FrameBuffer* m_viewportFrameBuffer = nullptr;
//SceneSnapshot* m_sceneSnapshot = nullptr;
//ComputeShader m_shader;
//GLuint m_resultTexture = 0;
//glm::ivec2 m_resultTextureResolution = { 1280, 720 };
//glm::ivec2 m_viewportResolution = { 1280, 720 };
//GLuint m_trianglesBuffer = 0;
//int32_t m_numTriangles = 0;
//GLuint m_materialsBuffer = 0;
//int32_t m_samples = 0;

std::string to_string(ViewportMode viewportMode) {
	switch (viewportMode) {
	case ViewportMode::Default: return "Default";
	case ViewportMode::Stereoscopic: return "Stereoscopic";
	case ViewportMode::VR: return "VR";
	default: return "Default";
	}
}

std::string to_string(RenderMode renderMode) {
	switch (renderMode) {
	case RenderMode::Forward: return "Forward";
	case RenderMode::Deffered:	return "Deffered";
	case RenderMode::PathTracing: return "PathTracing";
	case RenderMode::GPUPathTracing: return "GPUPathTracing";
	default: return "Wrong Render Mode";
	}
}

ViewportWindow::ViewportWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1280, 720 }), m_vrFrameBuffer({1280, 720}),
	m_viewportCamera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 100),
	m_cameraController(m_viewportCamera), m_stereoscopicFrameBuffer({ 1280, 720 }) {
	m_vrShader = ResourceManager::LoadShader("VRQuadVertexShader.glsl", "VRQuadFragmentShader.glsl");
	m_stereoscopicShader = ResourceManager::LoadShader("StereoscopicQuadVertexShader.glsl", "StereoscopicQuadFragmentShader.glsl");
}

void ViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("Viewport##")).c_str())) {
		if (ImGui::IsWindowFocused()) {
			m_cameraController.Update();
		}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		m_frameBuffer.Resize(glmViewportResolution);

		Scene* scene = SceneManager::GetScene().get();
		if (scene) {
			GLint originalViewport[4];
			glGetIntegerv(GL_VIEWPORT, originalViewport);

			Camera camera = m_viewportCamera;
			if (!m_useCameraResolution) {
				camera.SetResolution(m_frameBuffer.m_resolution);
			}

			if (m_mode == ViewportMode::Stereoscopic) {
				Camera leftCamera = camera;
				leftCamera.GetTransform().Translate(leftCamera.GetTransform().GetRight() * -m_stereoscopicDistance / 2.0f);
				Camera rightCamera = camera;
				rightCamera.GetTransform().Translate(rightCamera.GetTransform().GetRight() * m_stereoscopicDistance / 2.0f);

				if (m_renderMode == RenderMode::Deffered) {
					m_defferedRenderer.DrawFrame(scene, &leftCamera);
					m_secondaryDefferedRenderer.DrawFrame(scene, &rightCamera);
					m_stereoscopicFrameBuffer.Bind();
					m_stereoscopicFrameBuffer.Resize(camera.GetResolution());
					m_stereoscopicFrameBuffer.ResizeViewport();
					m_stereoscopicFrameBuffer.Clear();
					m_stereoscopicShader.Bind();
					m_stereoscopicShader.SetTexture("textureLeft", m_defferedRenderer.m_frameBuffer.m_texture, 0);
					m_stereoscopicShader.SetTexture("textureRight", m_secondaryDefferedRenderer.m_frameBuffer.m_texture, 1);
					m_stereoscopicShader.SetUniform1f("uDistance", m_stereoscopicDistance);
					m_stereoscopicShader.SetUniform3f("uBalance", m_stereoscopicBalance);
					m_stereoscopicShader.SetUniform3f("uScale", m_stereoscopicScale);
					ResourceManager::GetQuadMesh()->Draw();
					m_stereoscopicFrameBuffer.Unbind();
				}
				else {
					m_forwardRenderer.DrawFrame(scene, &leftCamera);
					m_secondaryForwardRenderer.DrawFrame(scene, &rightCamera);
					m_stereoscopicFrameBuffer.Bind();
					m_stereoscopicFrameBuffer.Resize(camera.GetResolution());
					m_stereoscopicFrameBuffer.ResizeViewport();
					m_stereoscopicFrameBuffer.Clear();
					m_stereoscopicShader.Bind();
					m_stereoscopicShader.SetTexture("textureLeft", m_forwardRenderer.m_frameBuffer.m_texture, 0);
					m_stereoscopicShader.SetTexture("textureRight", m_secondaryForwardRenderer.m_frameBuffer.m_texture, 1);
					m_stereoscopicShader.SetUniform1f("uDistance", m_stereoscopicDistance);
					m_stereoscopicShader.SetUniform3f("uBalance", m_stereoscopicBalance);
					m_stereoscopicShader.SetUniform3f("uScale", m_stereoscopicScale);
					ResourceManager::GetQuadMesh()->Draw();
					m_stereoscopicFrameBuffer.Unbind();
				}

				m_frameBuffer.Bind();
				m_frameBuffer.ResizeViewport();
				m_frameBuffer.Clear();
				GlobalRenderer::DrawTextureFitted(m_stereoscopicFrameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
				m_frameBuffer.Unbind();
			}
			else if (m_mode == ViewportMode::VR) {
				Camera leftCamera = camera;
				leftCamera.GetTransform().Translate(leftCamera.GetTransform().GetRight() * -m_vrDistance / 2.0f);
				leftCamera.SetResolution({ camera.GetResolution().x / 2, camera.GetResolution().y });
				Camera rightCamera = camera;
				rightCamera.GetTransform().Translate(rightCamera.GetTransform().GetRight() * m_vrDistance / 2.0f);
				rightCamera.SetResolution({ camera.GetResolution().x / 2, camera.GetResolution().y });

				if (m_renderMode == RenderMode::Deffered) {
					m_defferedRenderer.DrawFrame(scene, &leftCamera);
					m_secondaryDefferedRenderer.DrawFrame(scene, &rightCamera);
					m_vrFrameBuffer.Bind();
					m_vrFrameBuffer.Resize(camera.GetResolution());
					m_vrFrameBuffer.ResizeViewport();
					m_vrFrameBuffer.Clear();
					m_vrShader.Bind();
					m_vrShader.SetTexture("textureLeft", m_defferedRenderer.m_frameBuffer.m_texture, 0);
					m_vrShader.SetTexture("textureRight", m_secondaryDefferedRenderer.m_frameBuffer.m_texture, 1);
					m_vrShader.SetUniform1f("uDistance", m_vrDistance);
					m_vrShader.SetUniform1f("uK", m_vrDistortion);
					ResourceManager::GetQuadMesh()->Draw();
					m_vrFrameBuffer.Unbind();
				}
				else {
					m_forwardRenderer.DrawFrame(scene, &leftCamera);
					m_secondaryForwardRenderer.DrawFrame(scene, &rightCamera);
					m_vrFrameBuffer.Bind();
					m_vrFrameBuffer.Resize(camera.GetResolution());
					m_vrFrameBuffer.ResizeViewport();
					m_vrFrameBuffer.Clear();
					m_vrShader.Bind();
					m_vrShader.SetTexture("textureLeft", m_forwardRenderer.m_frameBuffer.m_texture, 0);
					m_vrShader.SetTexture("textureRight", m_secondaryForwardRenderer.m_frameBuffer.m_texture, 1);
					m_vrShader.SetUniform1f("uDistance", m_vrDistance);
					m_vrShader.SetUniform1f("uK", m_vrDistortion);
					ResourceManager::GetQuadMesh()->Draw();
					m_vrFrameBuffer.Unbind();
				}

				m_frameBuffer.Bind();
				m_frameBuffer.ResizeViewport();
				m_frameBuffer.Clear();
				GlobalRenderer::DrawTextureFitted(m_vrFrameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
				m_frameBuffer.Unbind();
			}
			else {
				if (m_renderMode == RenderMode::Forward) {
					m_forwardRenderer.DrawFrame(scene, &camera);
					m_frameBuffer.Bind();
					m_frameBuffer.ResizeViewport();
					m_frameBuffer.Clear();
					GlobalRenderer::DrawTextureFitted(m_forwardRenderer.m_frameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
					m_frameBuffer.Unbind();
				}
				else if (m_renderMode == RenderMode::Deffered) {
					m_defferedRenderer.DrawFrame(scene, &camera);
					m_frameBuffer.Bind();
					m_frameBuffer.ResizeViewport();
					m_frameBuffer.Clear();
					GlobalRenderer::DrawTextureFitted(m_defferedRenderer.m_frameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
					m_frameBuffer.Unbind();
				}
				else if (m_renderMode == RenderMode::PathTracing) {
					std::shared_ptr<SceneSnapshot> sceneSnapshot = SceneManager::GetSceneSnapshot();
					if (sceneSnapshot && (sceneSnapshot != m_pathTracingRenderer.m_sceneSnapshot || camera != m_pathTracingRenderer.m_camera)) {
						m_pathTracingRenderer.StopRender();
						m_pathTracingRenderer.StartRender(sceneSnapshot, camera);
					}
					m_pathTracingRenderer.DrawFrame();
					if (m_pathTracingRenderer.GetSamplesCount() < 2) {
						m_forwardRenderer.DrawFrame(scene, &camera);
					}
					m_frameBuffer.Bind();
					m_frameBuffer.ResizeViewport();
					m_frameBuffer.Clear();
					if (m_pathTracingRenderer.GetSamplesCount() < 2) {
						GlobalRenderer::DrawTextureFitted(m_forwardRenderer.m_frameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
						glClear(GL_DEPTH_BUFFER_BIT);
					}
					GlobalRenderer::DrawTextureFitted(m_pathTracingRenderer.m_frameBuffer.m_texture, m_pathTracingRenderer.m_frameBuffer.m_resolution, glmViewportResolution);
					m_frameBuffer.Unbind();
				}
				if (m_renderMode == RenderMode::GPUPathTracing) {
					m_forwardRenderer.DrawFrame(scene, &camera);
					m_frameBuffer.Bind();
					m_frameBuffer.ResizeViewport();
					m_frameBuffer.Clear();
					GlobalRenderer::DrawTextureFitted(m_forwardRenderer.m_frameBuffer.m_texture, camera.GetResolution(), glmViewportResolution);
					m_frameBuffer.Unbind();

					//ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
					//ImGui::SetNextWindowSize(viewportResolution);
					//glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
					//if (glmViewportResolution != m_viewportResolution) {
					//	UpdateViewportResolution(glmViewportResolution);
					//}
					//
					//if (ImGui::IsWindowFocused()) {
					//	if (m_cameraController.Update()) {
					//		m_samples = 0;
					//	}
					//}
					//
					//if (m_numTriangles > 0) {
					//	m_shader.Bind();
					//	m_shader.SetUniform1i("uNumTriangles", m_numTriangles);
					//	m_shader.SetUniform1i("uTime", (GLuint)Time::lastTime.count());
					//	m_shader.SetUniform1i("uReset", m_samples == 0);
					//	m_shader.SetUniformMat4f("uViewMatrix", m_viewportCamera.GetInverseViewMatrix());
					//	glDispatchCompute((GLuint)m_resultTextureResolution.x / 8, (GLuint)m_resultTextureResolution.y / 4, 1);
					//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
					//	m_samples++;
					//}
					//
					//m_viewportFrameBuffer->Bind();
					//glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//m_app.GetDefaultRenderer()->DrawTexture(m_resultTexture, m_resultTextureResolution, m_viewportResolution, m_samples);
					//m_viewportFrameBuffer->Unbind();
					//m_app.RestoreViewportSize();
					//
					//ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
					//
					// GPU RAY TRACER UPDATE SCENE
					//m_sceneSnapshot = m_app.GetSceneSnapshot();
					//if (!m_sceneSnapshot) {
					//	return;
					//}
					//
					//std::vector<Triangle>& triangles = m_sceneSnapshot->GetTriangles();
					//std::vector<AlignedTriangle> alignedTriangles;
					//alignedTriangles.resize(triangles.size());
					//for (size_t i = 0; i < triangles.size(); i++) {
					//	alignedTriangles[i] = AlignedTriangle(triangles[i]);
					//}
					//m_numTriangles = (int32_t)alignedTriangles.size();
					//int32_t trianglesBufferSize = m_numTriangles * sizeof(alignedTriangles[0]);
					//
					//if (!m_trianglesBuffer) {
					//	glGenBuffers(1, &m_trianglesBuffer);
					//}
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_trianglesBuffer);
					//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_trianglesBuffer, 0, trianglesBufferSize);
					//glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesBufferSize, alignedTriangles.data(), GL_STATIC_READ);
					//
					//std::vector<Material>& materials = ResourceManager::GetMaterials();
					//std::vector<AlignedMaterial> alignedMaterials;
					//alignedMaterials.resize(materials.size());
					//for (size_t i = 0; i < materials.size(); i++) {
					//	alignedMaterials[i] = AlignedMaterial(materials[i]);
					//}
					//int32_t materialsBufferSize = (int32_t)materials.size() * sizeof(AlignedMaterial);
					//
					//if (!m_materialsBuffer) {
					//	glGenBuffers(1, &m_materialsBuffer);
					//}
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialsBuffer);
					//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_materialsBuffer, 0, materialsBufferSize);
					//glBufferData(GL_SHADER_STORAGE_BUFFER, materialsBufferSize, alignedMaterials.data(), GL_STATIC_READ);
					//
					//m_samples = 0;
				}
			}

			glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);

			ImGui::Image((void*)(uint64_t)m_frameBuffer.m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

ViewportMode ViewportWindow::GetViewportMode() const {
	return m_mode;
}

void ViewportWindow::SetViewportMode(ViewportMode mode) {
	m_mode = mode;
}

RenderMode ViewportWindow::GetRenderMode() const {
	return m_renderMode;
}

void ViewportWindow::SetRenderMode(RenderMode renderMode) {
	if (m_renderMode == RenderMode::PathTracing) {
		m_pathTracingRenderer.StopRender();
	}
	m_renderMode = renderMode;
	if (m_renderMode == RenderMode::PathTracing) {
		m_pathTracingRenderer.StartRender(SceneManager::GetSceneSnapshot(), m_viewportCamera);
	}
	if (m_renderMode == RenderMode::PathTracing || m_renderMode == RenderMode::GPUPathTracing) {
		SetViewportMode(ViewportMode::Default);
	}
}

Float ViewportWindow::GetStereoscopicDistance() const {
	return m_stereoscopicDistance;
}

void ViewportWindow::SetStereoscopicDistance(Float distance) {
	m_stereoscopicDistance = distance;
}

Vec3 ViewportWindow::GetStereoscopicBalance() const {
	return m_stereoscopicBalance;
}

void ViewportWindow::SetStereoscopicBalance(Vec3 balance) {
	m_stereoscopicBalance = balance;
}

Vec3 ViewportWindow::GetStereoscopicScale() const {
	return m_stereoscopicScale;
}

void ViewportWindow::SetStereoscopicScale(Vec3 scale) {
	m_stereoscopicScale = scale;
}

Float ViewportWindow::GetVRDistance() const {
	return m_vrDistance;
}

void ViewportWindow::SetVRDistance(Float distance) {
	m_vrDistance = distance;
}

Float ViewportWindow::GetVRDistortion() const {
	return m_vrDistortion;
}

void ViewportWindow::SetVRDistortion(Float distortion) {
	m_vrDistortion = distortion;
}

const FrameBuffer& ViewportWindow::GetFrameBuffer() {
	return m_frameBuffer;
}
