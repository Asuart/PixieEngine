#include "pch.h"
#include "DefferedLightingNode.h"

const glm::ivec2 initialResolution = { 1280, 720 };

DefferedLightingNode::DefferedLightingNode() :
	ShaderNode("Deffered Lighting"),
	m_frame(initialResolution, GL_RGB, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE),
	m_LTC1Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC1, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_LINEAR),
	m_LTC2Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC2, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_LINEAR),
	m_noiseTexture(TextureGenerator::SSAONoiseTexture(SSAONoiseResolution)) {

	m_inputs.push_back({ *this, "Albedo", ShaderNodeIOType::textureRGB });
	m_inputs.push_back({ *this, "Normal", ShaderNodeIOType::textureRGB });
	m_inputs.push_back({ *this, "Position", ShaderNodeIOType::textureRGB });
	m_inputs.push_back({ *this, "Specular", ShaderNodeIOType::textureR });
	m_inputs.push_back({ *this, "Metallic", ShaderNodeIOType::textureR });
	m_inputs.push_back({ *this, "Roughness", ShaderNodeIOType::textureR });
	m_inputs.push_back({ *this, "AO", ShaderNodeIOType::textureR });

	m_outputs.push_back({ *this, "Frame", ShaderNodeIOType::textureRGB, &m_frame });

	m_program = ResourceManager::LoadShader("DefferedLightingNodeVertexShader.glsl", "DefferedLightingNodeFragmentShader.glsl");

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frame.m_id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DefferedLightingNode::Process(const Scene& scene, const Camera& camera) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glViewport(0, 0, initialResolution.x, initialResolution.y);
	glClear(GL_COLOR_BUFFER_BIT);
	m_program.Bind();
	m_program.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	m_program.SetTexture("gAlbedo", GetInputTexture("Albedo"), 0);
	m_program.SetTexture("gNormal", GetInputTexture("Normal"), 1);
	m_program.SetTexture("gPosition", GetInputTexture("Position"), 2);
	m_program.SetTexture("gSpecular", GetInputTexture("Specular"), 3);
	m_program.SetTexture("gMetallic", GetInputTexture("Metallic"), 4);
	m_program.SetTexture("gRoughness", GetInputTexture("Roughness"), 5);
	m_program.SetTexture("LTC1", m_LTC1Texture.m_id, 6);
	m_program.SetTexture("LTC2", m_LTC2Texture.m_id, 7);
	//m_program.SetTexture("ssaoTexture", m_ssaoBuffer.m_texture, 8);
	SetupLights(scene);
	ResourceManager::GetQuadMesh()->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void DefferedLightingNode::SetupLights(const Scene& scene) {
	// Point lights
	const size_t MaxPointLights = 32;
	int32_t nPointLights = 0;
	const std::vector<PointLightComponent*>& pointLights = scene.GetPointLights();
	for (size_t i = 0; i < pointLights.size() && i < MaxPointLights; i++) {
		const std::string base = std::string("pointLights[") + std::to_string(nPointLights) + std::string("].");
		m_program.SetUniform3f(base + std::string("position"), pointLights[i]->GetParent()->GetTransform().GetPosition());
		m_program.SetUniform3f(base + std::string("emission"), pointLights[i]->GetEmission());
		nPointLights++;
	}
	m_program.SetUniform1i("nPointLights", nPointLights);

	// Area lights
	const size_t MaxAreaLights = 32;
	int32_t nAreaLights = 0;
	const std::vector<AreaLightComponent*>& areaLights = scene.GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		MeshComponent* meshComponent = areaLights[i]->GetParent()->GetComponent<MeshComponent>();
		if (!meshComponent) continue;
		Mesh* mesh = meshComponent->GetMesh();
		if (!mesh) continue;
		for (int32_t firstIndex = 0; firstIndex < mesh->m_indices.size() && nAreaLights < MaxAreaLights; firstIndex += 3) {
			const std::string base = std::string("areaLights[") + std::to_string(nAreaLights) + std::string("].");
			m_program.SetUniform3f(base + std::string("emission"), areaLights[i]->GetEmission());
			m_program.SetUniform1i(base + std::string("twoSided"), 1);
			m_program.SetUniform3f(base + std::string("points[0]"), mesh->m_vertices[mesh->m_indices[firstIndex + 0]].position);
			m_program.SetUniform3f(base + std::string("points[1]"), mesh->m_vertices[mesh->m_indices[firstIndex + 1]].position);
			m_program.SetUniform3f(base + std::string("points[2]"), mesh->m_vertices[mesh->m_indices[firstIndex + 2]].position);
			nAreaLights++;
		}
		if (nAreaLights >= MaxAreaLights) break;
	}
	m_program.SetUniform1i("nAreaLights", nAreaLights);
}
