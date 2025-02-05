#include "pch.h"
#include "DefferedRenderNode.h"

DefferedRenderNode::DefferedRenderNode() :
	ShaderNode("Deffered Render") {
	m_outputs.push_back({ *this, "Albedo", ShaderNodeIOType::textureRGB, &m_albedo });
	m_outputs.push_back({ *this, "Normal", ShaderNodeIOType::textureRGB, &m_normal });
	m_outputs.push_back({ *this, "Position", ShaderNodeIOType::textureRGB, &m_position });
	m_outputs.push_back({ *this, "Specular", ShaderNodeIOType::textureR, &m_specular });
	m_outputs.push_back({ *this, "Metallic", ShaderNodeIOType::textureR, &m_metallic });
	m_outputs.push_back({ *this, "Roughness", ShaderNodeIOType::textureR, &m_roughness });
	m_outputs.push_back({ *this, "Depth", ShaderNodeIOType::textureR, &m_depth });

	m_program = ResourceManager::LoadShader("DefferedRenderNodeVertexShader.glsl", "DefferedRenderNodeFragmentShader.glsl");

	const glm::ivec2 resolution = { 1280, 720 };

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_albedo);
	glBindTexture(GL_TEXTURE_2D, m_albedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedo, 0);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal, 0);

	glGenTextures(1, &m_position);
	glBindTexture(GL_TEXTURE_2D, m_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_position, 0);

	glGenTextures(1, &m_specular);
	glBindTexture(GL_TEXTURE_2D, m_specular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_specular, 0);

	glGenTextures(1, &m_metallic);
	glBindTexture(GL_TEXTURE_2D, m_metallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_metallic, 0);

	glGenTextures(1, &m_roughness);
	glBindTexture(GL_TEXTURE_2D, m_roughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution.x, resolution.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_roughness, 0);

	GLuint attachments[6] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5
	};
	glDrawBuffers(6, attachments);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
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

	m_program.SetTexture("gAlbedo", m_albedo, 0);
	m_program.SetTexture("gNormal", m_normal, 1);
	m_program.SetTexture("gPosition", m_position, 2);
	m_program.SetTexture("gSpecular", m_specular, 3);
	m_program.SetTexture("gMetallic", m_metallic, 4);
	m_program.SetTexture("gRoughness", m_roughness, 5);

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
