#include "pch.h"
#include "DefferedRenderer.h"
#include "Resources/TextureGenerator.h"
#include "Log.h"

DefferedRenderer::DefferedRenderer() :
	m_gBuffer({ 1280, 720 }), m_frameBuffer({ 1280, 720 }),
	m_ssaoBuffer({ 1280, 720 }), m_ssaoBlurBuffer({ 1280, 720 }),
	m_LTC1Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC1, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear),
	m_LTC2Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC2, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear),
	m_noiseTexture(TextureGenerator::SSAONoiseTexture(SSAONoiseResolution)) {
	m_shader = ShaderManager::LoadShader("Deffered");
	m_ssaoShader = ShaderManager::LoadShader("SSAO");
	m_ssaoBlurShader = ShaderManager::LoadShader("SSAOBlur");
	m_lightingShader = ShaderManager::LoadShader("LightingPass");
}

void DefferedRenderer::DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	// Fill GBuffer
	m_gBuffer.Bind();
	m_gBuffer.ResizeViewport();
	m_gBuffer.Clear();
	m_shader.Bind();
	m_shader.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	m_shader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_shader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	DrawObject(scene->GetRootObject());
	m_gBuffer.Unbind();

	// Generate SSAO Texture
	m_ssaoBuffer.Bind();
	m_ssaoBuffer.ResizeViewport();
	m_ssaoBuffer.Clear();
	m_ssaoShader.Bind();
	m_ssaoShader.SetUniform3fv("ssaoKernel", &m_ssaoKernel.vectors[0][0], (int32_t)m_ssaoKernel.Size());
	m_ssaoShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_ssaoShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	m_ssaoShader.SetTexture("gPositionRoughness", m_gBuffer.m_positionRoughness, 1);
	m_ssaoShader.SetTexture("gNormalMetallic", m_gBuffer.m_normalMetallic, 2);
	m_ssaoShader.SetTexture("noiseTexture", m_noiseTexture.GetHandle(), 3);
	m_ssaoShader.SetUniform2f("noiseScale", glm::vec2(m_frameBuffer.GetResolution()) / glm::vec2(SSAONoiseResolution));
	m_ssaoShader.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	RenderEngine::DrawQuad();
	m_ssaoBuffer.Unbind();

	// Blur SSAO Texture
	m_ssaoBlurBuffer.Bind();
	m_ssaoBlurBuffer.ResizeViewport();
	m_ssaoBlurBuffer.Clear();
	m_ssaoBlurShader.Bind();
	m_ssaoBlurShader.SetTexture("inputTexture", m_ssaoBuffer.m_texture, 0);
	m_ssaoBlurShader.SetUniform1i("uBlurRange", 2);
	RenderEngine::DrawQuad();
	m_ssaoBlurBuffer.Unbind();

	// Lighting Pass
	m_frameBuffer.Bind();
	m_frameBuffer.ResizeViewport();
	m_frameBuffer.Clear();
	m_lightingShader.Bind();
	m_lightingShader.SetUniform3f("cameraPos", camera.GetTransform().GetPosition());
	m_lightingShader.SetTexture("gAlbedoSpec", m_gBuffer.m_albedoSpec, 0);
	m_lightingShader.SetTexture("gPositionRoughness", m_gBuffer.m_positionRoughness, 1);
	m_lightingShader.SetTexture("gNormalMetallic", m_gBuffer.m_normalMetallic, 2);
	m_lightingShader.SetTexture("LTC1", m_LTC1Texture.GetHandle(), 3);
	m_lightingShader.SetTexture("LTC2", m_LTC2Texture.GetHandle(), 4);
	m_lightingShader.SetTexture("ssaoTexture", m_ssaoBuffer.m_texture, 5);
	SetupLights(scene);
	RenderEngine::DrawQuad();
	m_frameBuffer.Unbind();

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void DefferedRenderer::SetResolution(glm::ivec2 resolution) {
	m_frameBuffer.Resize(resolution);
	m_gBuffer.Resize(resolution);
	m_ssaoBuffer.Resize(resolution);
	m_ssaoBlurBuffer.Resize(resolution);
}

glm::ivec2 DefferedRenderer::GetResolution() const {
	return m_frameBuffer.GetResolution();
}

void DefferedRenderer::DrawObject(SceneObject* object, glm::mat4 parentTransform) const {
	if (!object) return;

	glm::mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		if (const std::array<glm::mat4, cMaxBonesPerModel>* matrices = animatorComponent->GetBoneMatrices()) {
			m_shader.SetUniformMat4fv("finalBonesMatrices", &(*matrices)[0][0][0], (int32_t)matrices->size());
		}
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_shader.SetUniformMat4f("mModel", objectTransform);
		if (const MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			std::shared_ptr<Material> material = materialComponent->GetMaterial();
			SetupMaterial(material);
			mesh->Draw();
		}
		else {
			DebugLog::Warning("Trying to render mesh without set material");
		}
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_shader.SetUniformMat4f("mModel", glm::scale(objectTransform, glm::vec3(sphere->GetRadius())));
		if (const MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
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

void DefferedRenderer::SetupLights(std::shared_ptr<Scene> scene) const {
	// Point lights
	const size_t MaxPointLights = 32;
	int32_t nPointLights = 0;
	const std::vector<PointLightComponent*>& pointLights = scene->GetPointLights();
	for (size_t i = 0; i < pointLights.size() && i < MaxPointLights; i++) {
		const std::string base = std::string("pointLights[") + std::to_string(nPointLights) + std::string("].");
		m_lightingShader.SetUniform3f(base + std::string("position"), pointLights[i]->GetParent()->GetTransform().GetPosition());
		m_lightingShader.SetUniform3f(base + std::string("emission"), pointLights[i]->GetEmission());
		nPointLights++;
	}
	m_lightingShader.SetUniform1i("nPointLights", nPointLights);

	// Area lights
	const size_t MaxAreaLights = 32;
	int32_t nAreaLights = 0;
	const std::vector<DiffuseAreaLightComponent*>& areaLights = scene->GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		MeshComponent* meshComponent = areaLights[i]->GetParent()->GetComponent<MeshComponent>();
		if (!meshComponent) continue;
		const Mesh& mesh = areaLights[i]->GetMesh();
		for (int32_t firstIndex = 0; firstIndex < mesh.m_indices.size() && nAreaLights < MaxAreaLights; firstIndex += 3) {
			const std::string base = std::string("areaLights[") + std::to_string(nAreaLights) + std::string("].");
			m_lightingShader.SetUniform3f(base + std::string("emission"), areaLights[i]->GetEmission());
			m_lightingShader.SetUniform1i(base + std::string("twoSided"), 1);
			m_lightingShader.SetUniform3f(base + std::string("points[0]"), mesh.m_vertices[mesh.m_indices[firstIndex + 0]].position);
			m_lightingShader.SetUniform3f(base + std::string("points[1]"), mesh.m_vertices[mesh.m_indices[firstIndex + 1]].position);
			m_lightingShader.SetUniform3f(base + std::string("points[2]"), mesh.m_vertices[mesh.m_indices[firstIndex + 2]].position);
			nAreaLights++;
		}
		if (nAreaLights >= MaxAreaLights) break;
	}
	m_lightingShader.SetUniform1i("nAreaLights", nAreaLights);
}

void DefferedRenderer::SetupMaterial(std::shared_ptr<Material> material) const {
	m_shader.SetUniform3f("albedo", material->m_albedo);
	m_shader.SetTexture("albedoTexture", material->m_albedoTexture.GetHandle(), 4);
}

GLuint DefferedRenderer::GetFrameHandle() const {
	return m_frameBuffer.GetColorHandle();
}

AntiAliasing DefferedRenderer::GetAntiAliasing() const {
	return AntiAliasing::None;
}

void DefferedRenderer::SetAntiAliasing(AntiAliasing) {}
