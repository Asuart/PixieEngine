#include "pch.h"
#include "ForwardRenderer.h"
#include "LTC_Matrix.h"

ForwardRenderer::ForwardRenderer() :
	m_frameBuffer({1280, 720}) {
	m_defaultShader = ResourceManager::LoadShader("PhysicallyBasedVertexShader.glsl", "PhysicallyBasedFragmentShader.glsl");
	m_LTC1Texture = LoadLTCTexture(LTC1);
	m_LTC2Texture = LoadLTCTexture(LTC2);
}

void ForwardRenderer::DrawFrame(Scene* scene, Camera* camera) {
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	m_frameBuffer.Resize(camera->GetResolution());
	m_frameBuffer.Bind();
	m_frameBuffer.ResizeViewport();
	m_frameBuffer.Clear();

	m_defaultShader.Bind();
	SetupCamera(camera);
	SetupLights(scene);
	DrawObject(scene->GetRootObject());
	m_frameBuffer.Unbind();

	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void ForwardRenderer::DrawObject(SceneObject* object, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
		animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
		m_defaultShader.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], boneMatricesBuffer.size());
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_defaultShader.SetUniformMat4f("mModel", objectTransform);
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

void ForwardRenderer::SetupCamera(Camera* camera) {
	m_defaultShader.SetUniform3f("cameraPos", camera->GetTransform().GetPosition());
	m_defaultShader.SetUniformMat4f("mView", camera->GetViewMatrix());
	m_defaultShader.SetUniformMat4f("mProjection", camera->GetProjectionMatrix());
}

void ForwardRenderer::SetupLights(Scene* scene) {
	// Point lights
	const size_t MaxPointLights = 32;
	int32_t nPointLights = 0;
	const std::vector<PointLightComponent*>& pointLights = scene->GetPointLights();
	for (size_t i = 0; i < pointLights.size() && i < MaxPointLights; i++) {
		const std::string base = std::string("pointLights[") + std::to_string(nPointLights) + std::string("].");
		m_defaultShader.SetUniform3f(base + std::string("position"), pointLights[i]->GetParent()->GetTransform().GetPosition());
		m_defaultShader.SetUniform3f(base + std::string("emission"), pointLights[i]->GetEmission());
		nPointLights++;
	}
	m_defaultShader.SetUniform1i("nPointLights", nPointLights);

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
			m_defaultShader.SetUniform3f(base + std::string("emission"), areaLights[i]->GetEmission());
			m_defaultShader.SetUniform1i(base + std::string("twoSided"), 1);
			m_defaultShader.SetUniform3f(base + std::string("points[0]"), mesh->m_vertices[mesh->m_indices[firstIndex + 0]].position);
			m_defaultShader.SetUniform3f(base + std::string("points[1]"), mesh->m_vertices[mesh->m_indices[firstIndex + 1]].position);
			m_defaultShader.SetUniform3f(base + std::string("points[2]"), mesh->m_vertices[mesh->m_indices[firstIndex + 2]].position);
			nAreaLights++;
		}
		if (nAreaLights >= MaxAreaLights) break;
	}
	m_defaultShader.SetUniform1i("nAreaLights", nAreaLights);

	m_defaultShader.SetTexture("LTC1", m_LTC1Texture, 1);
	m_defaultShader.SetTexture("LTC2", m_LTC2Texture, 2);
}

void ForwardRenderer::SetupMaterial(Material* material) {
	m_defaultShader.SetUniform3f("albedo", material->m_albedo.GetRGB());
	m_defaultShader.SetUniform1f("metallic", material->m_metallic);
	m_defaultShader.SetUniform1f("roughness", material->m_roughness);
	m_defaultShader.SetUniform1i("useDiffuseMap", material->m_albedoTexture != nullptr);
	if (material->m_albedoTexture) {
		m_defaultShader.SetTexture("albedoTexture", material->m_albedoTexture->id, 0);
	}
}

GLuint ForwardRenderer::LoadLTCTexture(const Float* matrixTable) {
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