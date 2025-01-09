#include "pch.h"
#include "DefferedRenderer.h"

DefferedRenderer::DefferedRenderer() :
	m_gBuffer({ 1280, 720 }), m_viewportFrameBuffer({ 1280, 720 }) {
	m_shader = ResourceManager::LoadShader("DefferedVertexShader.glsl", "DefferedFragmentShader.glsl");
	m_lightingShader = ResourceManager::LoadShader("LightingPassVertexShader.glsl", "LightingPassFragmentShader.glsl");
	m_LTC1Texture = LoadLTCTexture(LTC1);
	m_LTC2Texture = LoadLTCTexture(LTC2);
}

void DefferedRenderer::DrawFrame(Scene* scene, Camera* camera) {
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	m_gBuffer.Resize(camera->GetResolution());
	m_viewportFrameBuffer.Resize(camera->GetResolution());
	glViewport(0, 0, camera->GetResolution().x, camera->GetResolution().y);
	m_gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_shader.Bind();
	SetupCamera(camera);
	DrawObject(scene->GetRootObject());
	m_viewportFrameBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_lightingShader.Bind();

	m_lightingShader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	
	m_gBuffer.BindTextures();
	SetupLights(scene);
	m_lightingShader.SetUniform1i("gAlbedoSpec", 0);
	m_lightingShader.SetUniform1i("gPositionRoughness", 1);
	m_lightingShader.SetUniform1i("gNormalMetallic", 2);
	ResourceManager::GetQuadMesh()->Draw();

	m_viewportFrameBuffer.Unbind();
}

void DefferedRenderer::DrawObject(SceneObject* object, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
		animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
		m_shader.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], boneMatricesBuffer.size());
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

void DefferedRenderer::SetupCamera(Camera* camera) {
	m_shader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	m_shader.SetUniformMat4f("mView", camera->GetViewMatrix());
	m_shader.SetUniformMat4f("mProjection", camera->GetProjectionMatrix());
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

	m_lightingShader.SetTexture("LTC1", m_LTC1Texture, 3);
	m_lightingShader.SetTexture("LTC2", m_LTC2Texture, 4);
}

void DefferedRenderer::SetupMaterial(Material* material) {
	m_shader.SetUniform3f("albedo", material->m_albedo.GetRGB());
	m_shader.SetUniform1f("metallic", material->m_metallic);
	m_shader.SetUniform1f("roughness", material->m_roughness);
	m_shader.SetUniform1i("useDiffuseMap", material->m_albedoTexture != nullptr);
	if (material->m_albedoTexture) {
		m_shader.SetTexture("albedoTexture", material->m_albedoTexture->id, 3);
	}
}

GLuint DefferedRenderer::LoadLTCTexture(const float* matrixTable) {
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, matrixTable);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
