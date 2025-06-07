#include "pch.h"
#include "ForwardRenderer.h"
#include "LTC_Matrix.h"

ForwardRenderer::ForwardRenderer() :
	m_frameBuffer({ 1280, 720 }),
	m_fxaaFrameBuffer({ 1280, 720 }),
	m_msFrameBuffer({ 1280, 720 }, 1),
	m_LTC1Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC1, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear),
	m_LTC2Texture({ 64, 64 }, GL_RGBA, GL_RGBA, GL_FLOAT, (void*)LTC2, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear) {
	m_defaultShader = ShaderManager::LoadShader("PhysicallyBased");
	m_fxaaShader = ShaderManager::LoadShader("FXAA");
}

void ForwardRenderer::DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const {
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	glClearDepth(Infinity);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (m_useMultisampleFramebuffer) {
		m_msFrameBuffer.Bind();
		m_msFrameBuffer.ResizeViewport();
		m_msFrameBuffer.Clear();
	}
	else {
		m_frameBuffer.Bind();
		m_frameBuffer.ResizeViewport();
		m_frameBuffer.Clear();
	}

	m_defaultShader.Bind();
	SetupCamera(camera);
	SetupLights(scene);
	DrawObject(scene->GetRootObject());
	RenderEngine::DrawSkybox(camera, scene->GetSkybox().m_cubemapTexture.GetHandle());

	if (m_useMultisampleFramebuffer) {
		m_msFrameBuffer.Blit(m_frameBuffer);
	}

	glDisable(GL_DEPTH_TEST);
	if (m_useFXAA) {
		m_fxaaFrameBuffer.Bind();
		m_fxaaFrameBuffer.ResizeViewport();
		m_fxaaFrameBuffer.Clear();
		m_fxaaShader.Bind();
		m_fxaaShader.SetTexture("frameTexture", m_frameBuffer.GetColorHandle(), 0);
		m_fxaaShader.SetUniform2f("uTextureSize", glm::vec2(m_frameBuffer.GetResolution()));
		RenderEngine::DrawQuad();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void ForwardRenderer::SetResolution(glm::ivec2 resolution) {
	m_frameBuffer.Resize(resolution * m_ssaaScale);
	m_fxaaFrameBuffer.Resize(resolution);
	m_msFrameBuffer.Resize(resolution);
}

glm::ivec2 ForwardRenderer::GetResolution() const {
	return m_frameBuffer.GetResolution();
}

GLuint ForwardRenderer::GetFrameHandle() const {
	if (m_useFXAA) {
		return m_fxaaFrameBuffer.GetColorHandle();
	}
	else {
		return m_frameBuffer.GetColorHandle();
	}
}

AntiAliasing ForwardRenderer::GetAntiAliasing() const {
	return m_antiAliasing;
}

void ForwardRenderer::SetAntiAliasing(AntiAliasing mode) {
	m_ssaaScale = 1;
	m_useMultisampleFramebuffer = false;
	m_antiAliasing = mode;
	m_useFXAA = false;
	switch (m_antiAliasing) {
	case AntiAliasing::SSAAx2:
		m_ssaaScale = 2; 
		break;
	case AntiAliasing::SSAAx4:
		m_ssaaScale = 4; 
		break;
	case AntiAliasing::SSAAx8:
		m_ssaaScale = 8; 
		break;
	case AntiAliasing::MSAAx2:
		m_useMultisampleFramebuffer = true;
		m_msFrameBuffer.SetSampleCount(2);
		break;
	case AntiAliasing::MSAAx4:
		m_useMultisampleFramebuffer = true;
		m_msFrameBuffer.SetSampleCount(4);
		break;
	case AntiAliasing::MSAAx8:
		m_useMultisampleFramebuffer = true;
		m_msFrameBuffer.SetSampleCount(8);
		break;
	case AntiAliasing::MSAAx16:
		m_useMultisampleFramebuffer = true;
		m_msFrameBuffer.SetSampleCount(16);
		break;
	case AntiAliasing::FXAA: 
		m_useFXAA = true; 
		break;
	default: break;
	}
}

void ForwardRenderer::SetShader(Shader shader) {
	m_defaultShader = shader;
}

void ForwardRenderer::DrawObject(SceneObject* object, glm::mat4 parentTransform) const {
	if (!object) return;

	glm::mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		if (const std::array<glm::mat4, cMaxBonesPerModel>* matrices = animatorComponent->GetBoneMatrices()) {
			m_defaultShader.SetUniformMat4fv("finalBonesMatrices", &(*matrices)[0][0][0], (int32_t)matrices->size());
		}
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		m_defaultShader.SetUniformMat4f("mModel", objectTransform);
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			std::shared_ptr<Material> material = materialComponent->GetMaterial();
			m_defaultShader.SetMaterial(material);
			mesh->Draw();
		}
		else {
			DebugLog::Warning("Trying to render mesh without set material");
		}
	}
	if (const SphereComponent* sphere = object->GetComponent<SphereComponent>()) {
		m_defaultShader.SetUniformMat4f("mModel", glm::scale(objectTransform, glm::vec3(sphere->GetRadius())));
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			std::shared_ptr<Material> material = materialComponent->GetMaterial();
			m_defaultShader.SetMaterial(material);
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

void ForwardRenderer::SetupCamera(const Camera& camera) const {
	m_defaultShader.SetUniform3f("uCameraPos", camera.GetTransform().GetPosition());
	m_defaultShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_defaultShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
}

void ForwardRenderer::SetupLights(std::shared_ptr<Scene> scene) const {
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
	const std::vector<DiffuseAreaLightComponent*>& areaLights = scene->GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		MeshComponent* meshComponent = areaLights[i]->GetParent()->GetComponent<MeshComponent>();
		if (!meshComponent) continue;
		const Mesh& mesh = areaLights[i]->GetMesh();
		for (int32_t firstIndex = 0; firstIndex < mesh.m_indices.size() && nAreaLights < MaxAreaLights; firstIndex += 3) {
			const std::string base = std::string("areaLights[") + std::to_string(nAreaLights) + std::string("].");
			m_defaultShader.SetUniform3f(base + std::string("emission"), areaLights[i]->GetEmission());
			m_defaultShader.SetUniform1i(base + std::string("twoSided"), 1);
			m_defaultShader.SetUniform3f(base + std::string("points[0]"), mesh.m_vertices[mesh.m_indices[firstIndex + 0]].position);
			m_defaultShader.SetUniform3f(base + std::string("points[1]"), mesh.m_vertices[mesh.m_indices[firstIndex + 1]].position);
			m_defaultShader.SetUniform3f(base + std::string("points[2]"), mesh.m_vertices[mesh.m_indices[firstIndex + 2]].position);
			nAreaLights++;
		}
		if (nAreaLights >= MaxAreaLights) break;
	}
	m_defaultShader.SetUniform1i("nAreaLights", nAreaLights);

	m_defaultShader.SetTexture("LTC1", m_LTC1Texture.GetHandle(), 5);
	m_defaultShader.SetTexture("LTC2", m_LTC2Texture.GetHandle(), 6);
	m_defaultShader.SetTexture("brdfLUTMap", RenderEngine::GetBRDFLUTHandle(), 7);
	m_defaultShader.SetCubeMap("irradianceMap", scene->GetSkybox().m_lightmapTexture.GetHandle(), 8);
	m_defaultShader.SetCubeMap("prefilterMap", scene->GetSkybox().m_prefilteredTexture.GetHandle(), 9);
}
