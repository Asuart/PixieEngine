#include "pch.h"
#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() {
	m_defaultShader = CompileShader(PBR_VERTEX_SHADER_SOURCE, PBR_FRAGMENT_SHADER_SOURCE);
}

void DefaultRenderer::DrawFrame(Scene* scene, Camera* camera) {
	glUseProgram(m_defaultShader);

	SetupCamera(camera);
	SetupLights(scene);

	GLuint mModelLoc = glGetUniformLocation(m_defaultShader, "mModel");
	SceneObject* rootObject = scene->GetRootObject();
	DrawObject(rootObject, mModelLoc);
}

void DefaultRenderer::DrawObject(SceneObject* object, GLuint mModelLoc, Mat4 parentTransform) {
	if (!object) return;

	Mat4 objectTransform = parentTransform * object->GetTransform().GetMatrix();
	if (const MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		std::array<Mat4, MaxBonesPerModel> boneMatricesBuffer;
		animatorComponent->GetBoneMatrices(0.0f, boneMatricesBuffer);
		GLuint mat4Loc = glGetUniformLocation(m_defaultShader, "finalBonesMatrices");
		glUniformMatrix4(mat4Loc, (GLsizei)boneMatricesBuffer.size(), GL_FALSE, &boneMatricesBuffer[0][0][0]);
	}
	if (const MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		glUniformMatrix4(mModelLoc, 1, GL_FALSE, &objectTransform[0][0]);
		Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		SetupMaterial(material);
		mesh->Draw();
	}
	for (size_t i = 0; i < object->GetChildren().size(); i++) {
		DrawObject(object->GetChild(i), mModelLoc, objectTransform);
	}
}

void DefaultRenderer::SetupCamera(Camera* camera) {
	GLuint cameraPosLoc = glGetUniformLocation(m_defaultShader, "cameraPos");
	GLuint mViewLoc = glGetUniformLocation(m_defaultShader, "mView");
	GLuint mProjectioLoc = glGetUniformLocation(m_defaultShader, "mProjection");

	glm::fvec3 cameraPos = camera->GetTransform().GetPosition();
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4(mViewLoc, 1, GL_FALSE, &camera->GetViewMatrix()[0][0]);
	glUniformMatrix4(mProjectioLoc, 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
}

void DefaultRenderer::SetupLights(Scene* scene) {
	const size_t MaxLights = 4;

	GLuint positionsLoc = glGetUniformLocation(m_defaultShader, "lightPositions");
	GLuint colorsLoc = glGetUniformLocation(m_defaultShader, "lightColors");

	GLfloat positions[MaxLights * 3] = { 0 };
	GLfloat colors[MaxLights * 3] = { 0 };

	const std::vector<PointLightComponent*>& pointLights = scene->GetPointLights();
	for (size_t i = 0; i < pointLights.size() && i < MaxLights; i++) {
		glm::fvec3 center = pointLights[i]->GetParent()->GetTransform().GetPosition();
		positions[i * 3 + 0] = center.x;
		positions[i * 3 + 1] = center.y;
		positions[i * 3 + 2] = center.z;
		glm::fvec3 emission = pointLights[i]->GetEmission();
		colors[i * 3 + 0] = emission.x;
		colors[i * 3 + 1] = emission.y;
		colors[i * 3 + 2] = emission.z;
	}

	glUniform3fv(positionsLoc, MaxLights, positions);
	glUniform3fv(colorsLoc, MaxLights, colors);
}

void DefaultRenderer::SetupMaterial(Material* material) {
	GLuint albedoLoc = glGetUniformLocation(m_defaultShader, "albedo");
	GLuint metallicLoc = glGetUniformLocation(m_defaultShader, "metallic");
	GLuint roghnessLoc = glGetUniformLocation(m_defaultShader, "roughness");
	GLuint ambientOcclusionLoc = glGetUniformLocation(m_defaultShader, "ambientOcclusion");
	GLuint useDiffuseMapLoc = glGetUniformLocation(m_defaultShader, "useDiffuseMap");

	glUniform3(albedoLoc, material->m_albedo.GetRGBValue());
	glUniform1(metallicLoc, material->m_metallic);
	glUniform1(roghnessLoc, material->m_roughness);
	glUniform1(ambientOcclusionLoc, material->m_ambiendOcclusion);
	glUniform1i(useDiffuseMapLoc, material->m_albedoTexture != nullptr);
	if (material->m_albedoTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->m_albedoTexture->id);
	}
}
