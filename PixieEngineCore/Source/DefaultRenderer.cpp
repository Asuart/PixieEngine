#include "pch.h"
#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() {
	m_defaultShader = ResourceManager::CompileShader(PBR_VERTEX_SHADER_SOURCE, PBR_FRAGMENT_SHADER_SOURCE);
	m_quadShader = ResourceManager::CompileShader(QUAD_VERTEX_SHADER_SOURCE, QUAD_FRAGMENT_SHADER_SOURCE);
}

void DefaultRenderer::DrawFrame(Scene* scene, Camera* camera) {
	glUseProgram(m_defaultShader);

	SetupCamera(camera);
	SetupLights(scene);

	GLuint mModelLoc = glGetUniformLocation(m_defaultShader, "mModel");
	SceneObject* rootObject = scene->GetRootObject();
	DrawObject(rootObject, mModelLoc);
}

void DefaultRenderer::DrawTexture(GLuint texture, glm::ivec2 textureResolution, glm::ivec2 viewportResolution, int32_t samples) {
	glUseProgram(m_quadShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(m_quadShader, "ourTexture"), 0);
	float textureAspect = Aspect(textureResolution);
	float viewportAspect = Aspect(viewportResolution);
	float posX, posY;
	float sizeX, sizeY;
	if (viewportAspect > textureAspect) {
		sizeY = 1.0f;
		sizeX = textureAspect / viewportAspect;
		posX = (1.0f - sizeX) * 0.5f;
		posY = 0.0f;
	}
	else {
		sizeX = 1.0f;
		sizeY = viewportAspect / textureAspect;
		posX = 0.0f;
		posY = (1.0f - sizeY) * 0.5f;
	}
	GLuint posLoc = glGetUniformLocation(m_quadShader, "uPos");
	GLuint sizeLoc = glGetUniformLocation(m_quadShader, "uSize");
	GLuint samplesLoc = glGetUniformLocation(m_quadShader, "uSamples");
	glUniform2f(posLoc, posX, posY);
	glUniform2f(sizeLoc, sizeX, sizeY);
	glUniform1f(samplesLoc, (Float)samples);

	ResourceManager::GetQuadMesh()->Draw();
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
		DrawObject(object->GetChild((int32_t)i), mModelLoc, objectTransform);
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
	GLuint useDiffuseMapLoc = glGetUniformLocation(m_defaultShader, "useDiffuseMap");

	glUniform3(albedoLoc, material->m_albedo.GetRGB());
	glUniform1(metallicLoc, material->m_metallic);
	glUniform1(roghnessLoc, material->m_roughness);
	glUniform1i(useDiffuseMapLoc, material->m_albedoTexture != nullptr);
	if (material->m_albedoTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->m_albedoTexture->id);
	}
}
