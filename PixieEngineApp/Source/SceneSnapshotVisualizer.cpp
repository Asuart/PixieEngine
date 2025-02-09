#include "pch.h"
#include "SceneSnapshotVisualizer.h"

SceneSnapshotVisualizer::SceneSnapshotVisualizer(glm::ivec2 resolution) {
	glGenFramebuffers(1, &m_opaqueFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_opaqueFBO);

	glGenTextures(1, &m_opaqueTexture);
	glBindTexture(GL_TEXTURE_2D, m_opaqueTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_opaqueTexture, 0);

	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Opaque framebuffer is not complete!" << std::endl;
	}

	glGenFramebuffers(1, &m_transparentFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_transparentFBO);

	glGenTextures(1, &m_accumTexture);
	glBindTexture(GL_TEXTURE_2D, m_accumTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &m_revealTexture);
	glBindTexture(GL_TEXTURE_2D, m_revealTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_accumTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_revealTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0); // opaque framebuffer's depth texture

	const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_solidShader = ResourceManager::LoadShader("NoLights");
	m_transparentShader = ResourceManager::LoadShader("Transparent");
	m_compositeShader = ResourceManager::LoadShader("TransparentComposition");
}

void SceneSnapshotVisualizer::Draw(Scene* scene, const SceneSnapshot& sceneSnapshot, const Camera& camera) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	glm::vec4 zeroFillerVec(0.0f);
	glm::vec4 oneFillerVec(1.0f);

	glViewport(0, 0, 1280, 720);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, m_opaqueFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_solidShader.Bind();
	m_solidShader.SetUniform3f("uCameraPos", camera.GetTransform().GetPosition());
	m_solidShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_solidShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	DrawObject(scene->GetRootObject());

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	glBindFramebuffer(GL_FRAMEBUFFER, m_transparentFBO);
	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

	m_transparentShader.Bind();
	m_transparentShader.SetUniform3f("uCameraPos", camera.GetTransform().GetPosition());
	m_transparentShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_transparentShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	for (size_t i = 0; i < sceneSnapshot.m_nodes.size(); i++) {
		Bounds3f bounds = Bounds3f(sceneSnapshot.m_nodes[i].pMin, sceneSnapshot.m_nodes[i].pMax);
		if (bounds.IsEmpty()) continue;
		m_transparentShader.SetUniformMat4f("mModel", glm::scale(glm::translate(bounds.Center()), bounds.Diagonal()));
		m_transparentShader.SetUniform4f("uColor", glm::vec4(0.0f, 0.0f, 1.0f, 0.05f));
		GlobalRenderer::DrawMesh(ResourceManager::GetCubeMesh());
	}

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, m_opaqueFBO);

	m_compositeShader.Bind();
	m_compositeShader.SetTexture("accum", m_accumTexture, 0);
	m_compositeShader.SetTexture("reveal", m_revealTexture, 1);
	GlobalRenderer::DrawMesh(ResourceManager::GetQuadMesh());

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void SceneSnapshotVisualizer::DrawObject(SceneObject* object, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_solidShader.SetUniformMat4f("mModel", objectTransform);
		Material* material = ResourceManager::GetDefaultMaterial();
		m_solidShader.SetUniform3f("uColor", material->m_albedo.GetRGB());
		m_solidShader.SetMaterial(material);
		mesh->Draw();
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_solidShader.SetUniformMat4f("mModel", glm::scale(objectTransform, Vec3(sphere->GetRadius())));
		Material* material = ResourceManager::GetDefaultMaterial();
		m_solidShader.SetUniform3f("uColor", material->m_albedo.GetRGB());
		m_solidShader.SetMaterial(material);
		sphere->Draw();
	}
	for (size_t i = 0; i < object->GetChildren().size(); i++) {
		DrawObject(object->GetChild((int32_t)i), objectTransform);
	}
}