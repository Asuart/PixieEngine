#include "pch.h"
#include "DefferedRenderer.h"

DefferedRenderer::DefferedRenderer() :
	m_gBuffer({ 1280, 720 }), m_frameBuffer({ 1280, 720 }),
	m_ssaoBuffer({ 1280, 720 }), m_ssaoBlurBuffer({ 1280, 720 }),
	m_LTC1Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC1, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_LINEAR),
	m_LTC2Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC2, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_LINEAR),
	m_noiseTexture(TextureGenerator::SSAONoiseTexture(SSAONoiseResolution)) {
	m_shader = ResourceManager::LoadShader("DefferedVertexShader.glsl", "DefferedFragmentShader.glsl");
	m_ssaoShader = ResourceManager::LoadShader("SSAOVertexShader.glsl", "SSAOFragmentShader.glsl");
	m_ssaoBlurShader = ResourceManager::LoadShader("SSAOBlurVertexShader.glsl", "SSAOBlurFragmentShader.glsl");
	m_lightingShader = ResourceManager::LoadShader("LightingPassVertexShader.glsl", "LightingPassFragmentShader.glsl");
}

void DefferedRenderer::DrawFrame(Scene* scene, Camera* camera) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	// Fill GBuffer
	m_gBuffer.Resize(camera->GetResolution());
	m_gBuffer.Bind();
	m_gBuffer.ResizeViewport();
	m_gBuffer.Clear();
	m_shader.Bind();
	m_shader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	m_shader.SetUniformMat4f("mView", camera->GetViewMatrix());
	m_shader.SetUniformMat4f("mProjection", camera->GetProjectionMatrix());
	DrawObject(scene->GetRootObject());
	m_gBuffer.Unbind();

	// Generate SSAO Texture
	m_ssaoBuffer.Resize(camera->GetResolution());
	m_ssaoBuffer.Bind();
	m_ssaoBuffer.ResizeViewport();
	m_ssaoBuffer.Clear();
	m_ssaoShader.Bind();
	m_ssaoShader.SetUniform3fv("ssaoKernel", &m_ssaoKernel.vectors[0][0], m_ssaoKernel.Size());
	m_ssaoShader.SetUniformMat4f("mView", camera->GetViewMatrix());
	m_ssaoShader.SetUniformMat4f("mProjection", camera->GetProjectionMatrix());
	m_ssaoShader.SetTexture("gPositionRoughness", m_gBuffer.m_positionRoughness, 1);
	m_ssaoShader.SetTexture("gNormalMetallic", m_gBuffer.m_normalMetallic, 2);
	m_ssaoShader.SetTexture("noiseTexture", m_noiseTexture.m_id, 3);
	m_ssaoShader.SetUniform2f("noiseScale", Vec2(camera->GetResolution()) / Vec2(SSAONoiseResolution));
	m_ssaoShader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	ResourceManager::GetQuadMesh()->Draw();
	m_ssaoBuffer.Unbind();

	// Blur SSAO Texture
	m_ssaoBlurBuffer.Resize(camera->GetResolution());
	m_ssaoBlurBuffer.Bind();
	m_ssaoBlurBuffer.ResizeViewport();
	m_ssaoBlurBuffer.Clear();
	m_ssaoBlurShader.Bind();
	m_ssaoBlurShader.SetTexture("inputTexture", m_ssaoBuffer.m_texture, 0);
	m_ssaoBlurShader.SetUniform1i("uBlurRange", 2);
	ResourceManager::GetQuadMesh()->Draw();
	m_ssaoBlurBuffer.Unbind();

	// Lighting Pass
	m_frameBuffer.Resize(camera->GetResolution());
	m_frameBuffer.Bind();
	m_frameBuffer.ResizeViewport();
	m_frameBuffer.Clear();
	m_lightingShader.Bind();
	m_lightingShader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	m_lightingShader.SetTexture("gAlbedoSpec", m_gBuffer.m_albedoSpec, 0);
	m_lightingShader.SetTexture("gPositionRoughness", m_gBuffer.m_positionRoughness, 1);
	m_lightingShader.SetTexture("gNormalMetallic", m_gBuffer.m_normalMetallic, 2);
	m_lightingShader.SetTexture("LTC1", m_LTC1Texture.m_id, 3);
	m_lightingShader.SetTexture("LTC2", m_LTC2Texture.m_id, 4);
	m_lightingShader.SetTexture("ssaoTexture", m_ssaoBuffer.m_texture, 5);
	SetupLights(scene);
	ResourceManager::GetQuadMesh()->Draw();
	m_frameBuffer.Unbind();

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void DefferedRenderer::DrawObject(SceneObject* object, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
		animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
		m_shader.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], (int32_t)boneMatricesBuffer.size());
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_shader.SetUniformMat4f("mModel", objectTransform);
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

void DefferedRenderer::SetupLights(Scene* scene) {
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
	const std::vector<AreaLightComponent*>& areaLights = scene->GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		MeshComponent* meshComponent = areaLights[i]->GetParent()->GetComponent<MeshComponent>();
		if (!meshComponent) continue;
		Mesh* mesh = meshComponent->GetMesh();
		if (!mesh) continue;
		for (int32_t firstIndex = 0; firstIndex < mesh->m_indices.size() && nAreaLights < MaxAreaLights; firstIndex += 3) {
			const std::string base = std::string("areaLights[") + std::to_string(nAreaLights) + std::string("].");
			m_lightingShader.SetUniform3f(base + std::string("emission"), areaLights[i]->GetEmission());
			m_lightingShader.SetUniform1i(base + std::string("twoSided"), 1);
			m_lightingShader.SetUniform3f(base + std::string("points[0]"), mesh->m_vertices[mesh->m_indices[firstIndex + 0]].position);
			m_lightingShader.SetUniform3f(base + std::string("points[1]"), mesh->m_vertices[mesh->m_indices[firstIndex + 1]].position);
			m_lightingShader.SetUniform3f(base + std::string("points[2]"), mesh->m_vertices[mesh->m_indices[firstIndex + 2]].position);
			nAreaLights++;
		}
		if (nAreaLights >= MaxAreaLights) break;
	}
	m_lightingShader.SetUniform1i("nAreaLights", nAreaLights);
}

void DefferedRenderer::SetupMaterial(Material* material) {
	m_shader.SetUniform3f("albedo", material->m_albedo.GetRGB());
	m_shader.SetUniform1f("metallic", material->m_metallic);
	m_shader.SetUniform1f("roughness", material->m_roughness);
	m_shader.SetTexture("albedoTexture", material->m_albedoTexture.GetID(), 3);
}
