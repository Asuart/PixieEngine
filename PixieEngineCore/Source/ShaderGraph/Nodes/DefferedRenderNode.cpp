#include "pch.h"
#include "DefferedRenderNode.h"
#include "SphereComponent.h"

const glm::ivec2 initialResolution = { 1280, 720 };

DefferedRenderNode::DefferedRenderNode() :
	ShaderNode("Deffered Render"),
	m_albedo(initialResolution, GL_RGB, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_normal(initialResolution, GL_RGB32F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_position(initialResolution, GL_RGB32F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_specular(initialResolution, GL_RED, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_metallic(initialResolution, GL_RED, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_roughness(initialResolution, GL_RED, GL_RED, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE) {
	m_outputs.push_back({ *this, "Albedo", ShaderNodeIOType::textureRGB, &m_albedo });
	m_outputs.push_back({ *this, "Normal", ShaderNodeIOType::textureRGB, &m_normal });
	m_outputs.push_back({ *this, "Position", ShaderNodeIOType::textureRGB, &m_position });
	m_outputs.push_back({ *this, "Specular", ShaderNodeIOType::textureR, &m_specular });
	m_outputs.push_back({ *this, "Metallic", ShaderNodeIOType::textureR, &m_metallic });
	m_outputs.push_back({ *this, "Roughness", ShaderNodeIOType::textureR, &m_roughness });
	m_outputs.push_back({ *this, "Depth", ShaderNodeIOType::textureR, &m_depth });

	m_program = ResourceManager::LoadShader("DefferedRenderNode");

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedo.m_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal.m_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_position.m_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_specular.m_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_metallic.m_id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_roughness.m_id, 0);

	GLuint attachments[6] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5
	};
	glDrawBuffers(6, attachments);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, initialResolution.x, initialResolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw "Failed to initializa FrameBuffer\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DefferedRenderNode::Process(const Scene& scene, const Camera& camera) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_program.Bind();

	m_program.SetTexture("gAlbedo", m_albedo.m_id, 0);
	m_program.SetTexture("gNormal", m_normal.m_id, 1);
	m_program.SetTexture("gPosition", m_position.m_id, 2);
	m_program.SetTexture("gSpecular", m_specular.m_id, 3);
	m_program.SetTexture("gMetallic", m_metallic.m_id, 4);
	m_program.SetTexture("gRoughness", m_roughness.m_id, 5);

	m_program.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	m_program.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_program.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	DrawObject(scene.GetRootObject(), Mat4(1.0f));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void DefferedRenderNode::DrawObject(SceneObject* object, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
		animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
		m_program.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], (int32_t)boneMatricesBuffer.size());
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_program.SetUniformMat4f("mModel", objectTransform);
		Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		SetupMaterial(material);
		mesh->Draw();
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_program.SetUniformMat4f("mModel", glm::scale(objectTransform, Vec3(sphere->GetRadius())));
		Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		SetupMaterial(material);
		sphere->Draw();
	}
	for (size_t i = 0; i < object->GetChildren().size(); i++) {
		DrawObject(object->GetChild((int32_t)i), objectTransform);
	}
}

void DefferedRenderNode::SetupMaterial(Material* material) {
	m_program.SetUniform3f("albedo", material->m_albedo.GetRGB());
	m_program.SetUniform1f("metallic", material->m_metallic);
	m_program.SetUniform1f("roughness", material->m_roughness);
	m_program.SetTexture("albedoTexture", material->m_albedoTexture.GetID(), 6);
}
