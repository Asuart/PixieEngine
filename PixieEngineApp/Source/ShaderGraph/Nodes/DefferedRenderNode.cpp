#include "pch.h"
#include "DefferedRenderNode.h"
#include "Scene/Components/SphereComponent.h"

const glm::ivec2 initialResolution = { 1280, 720 };

DefferedRenderNode::DefferedRenderNode() :
	ShaderNode("Deffered Render"),
	m_albedo(initialResolution, GL_RGB, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge),
	m_normal(initialResolution, GL_RGB32F, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge),
	m_position(initialResolution, GL_RGB32F, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge),
	m_specular(initialResolution, GL_RED, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge),
	m_metallic(initialResolution, GL_RED, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge),
	m_roughness(initialResolution, GL_RED, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge) {
	m_outputs.push_back({ *this, "Albedo", ShaderNodeIOType::textureRGB, &m_albedo });
	m_outputs.push_back({ *this, "Normal", ShaderNodeIOType::textureRGB, &m_normal });
	m_outputs.push_back({ *this, "Position", ShaderNodeIOType::textureRGB, &m_position });
	m_outputs.push_back({ *this, "Specular", ShaderNodeIOType::textureR, &m_specular });
	m_outputs.push_back({ *this, "Metallic", ShaderNodeIOType::textureR, &m_metallic });
	m_outputs.push_back({ *this, "Roughness", ShaderNodeIOType::textureR, &m_roughness });
	m_outputs.push_back({ *this, "Depth", ShaderNodeIOType::textureR, &m_depth });

	m_program = ShaderManager::LoadShader("DefferedRenderNode");

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedo.GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal.GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_position.GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_specular.GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_metallic.GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_roughness.GetHandle(), 0);

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
		throw "Failed to initializa FrameBuffer";
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

	m_program.SetTexture("gAlbedo", m_albedo.GetHandle(), 0);
	m_program.SetTexture("gNormal", m_normal.GetHandle(), 1);
	m_program.SetTexture("gPosition", m_position.GetHandle(), 2);
	m_program.SetTexture("gSpecular", m_specular.GetHandle(), 3);
	m_program.SetTexture("gMetallic", m_metallic.GetHandle(), 4);
	m_program.SetTexture("gRoughness", m_roughness.GetHandle(), 5);

	m_program.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	m_program.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_program.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	DrawObject(scene.GetRootObject(), glm::mat4(1.0f));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void DefferedRenderNode::DrawObject(SceneObject* object, glm::mat4 parentTransform) {
	if (!object) return;

	glm::mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		if (const std::array<glm::mat4, cMaxBonesPerModel>* matrices = animatorComponent->GetBoneMatrices()) {
			m_program.SetUniformMat4fv("finalBonesMatrices", &(*matrices)[0][0][0], (int32_t)matrices->size());
		}
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_program.SetUniformMat4f("mModel", objectTransform);
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			std::shared_ptr<Material> material = materialComponent->GetMaterial();
			SetupMaterial(material);
			mesh->Draw();
		}
		else {
			DebugLog::Warning("Trying to render mesh without set material");
		}
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_program.SetUniformMat4f("mModel", glm::scale(objectTransform, glm::vec3(sphere->GetRadius())));
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			std::shared_ptr<Material> material = materialComponent->GetMaterial();
			SetupMaterial(material);
			sphere->Draw();
		}
		else {
			DebugLog::Warning("Trying to render sphere without set material");
		}
	}
	for (size_t i = 0; i < object->GetChildren().size(); i++) {
		DrawObject(object->GetChild((int32_t)i), objectTransform);
	}
}

void DefferedRenderNode::SetupMaterial(std::shared_ptr<Material> material) {
	m_program.SetUniform3f("albedo", material->m_albedo);
	m_program.SetTexture("albedoTexture", material->m_albedoTexture.GetHandle(), 6);
}
