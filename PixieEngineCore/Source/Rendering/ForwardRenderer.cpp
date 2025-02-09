#include "pch.h"
#include "ForwardRenderer.h"
#include "LTC_Matrix.h"

ForwardRenderer::ForwardRenderer() :
	m_frameBuffer({1280, 720}),
	m_LTC1Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC1, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear),
	m_LTC2Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC2, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear) {
	m_defaultShader = ResourceManager::LoadShader("PhysicallyBased");
}

void ForwardRenderer::DrawFrame(Scene* scene, Camera* camera) {
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	m_frameBuffer.Resize(camera->GetResolution());
	m_frameBuffer.Bind();
	m_frameBuffer.ResizeViewport();
	m_frameBuffer.Clear();

	GlobalRenderer::DrawSkybox(*camera, scene->GetSkybox().m_cubemapTexture.GetHandle());

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
		m_defaultShader.SetUniformMat4fv("finalBonesMatrices", &boneMatricesBuffer[0][0][0], (int32_t)boneMatricesBuffer.size());
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_defaultShader.SetUniformMat4f("mModel", objectTransform);
		Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		m_defaultShader.SetMaterial(material);
		mesh->Draw();
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_defaultShader.SetUniformMat4f("mModel", glm::scale(objectTransform, Vec3(sphere->GetRadius())));
		Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		m_defaultShader.SetMaterial(material);
		sphere->Draw();
	}
	for (size_t i = 0; i < object->GetChildren().size(); i++) {
		DrawObject(object->GetChild((int32_t)i), objectTransform);
	}
}

void ForwardRenderer::SetupCamera(Camera* camera) {
	m_defaultShader.SetUniform3f("uCameraPos", camera->GetTransform().GetPosition());
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

	m_defaultShader.SetTexture("LTC1", m_LTC1Texture.GetHandle(), 5);
	m_defaultShader.SetTexture("LTC2", m_LTC2Texture.GetHandle(), 6);
	m_defaultShader.SetTexture("brdfLUTMap", ResourceManager::GetBRDFLookUpTexture().GetHandle(), 7);
	m_defaultShader.SetCubeMap("irradianceMap", scene->GetSkybox().m_lightmapTexture.GetHandle(), 8);
	m_defaultShader.SetCubeMap("prefilterMap", scene->GetSkybox().m_prefilteredTexture.GetHandle(), 9);
}
