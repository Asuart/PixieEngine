#include "pch.h"
#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer(const glm::ivec2& resolution, Scene* scene)
	: Renderer(resolution), m_scene(scene) {
	m_defaultShader = CompileShader(PBR_VERTEX_SHADER_SOURCE, PBR_FRAGMENT_SHADER_SOURCE);
}

void DefaultRenderer::SetScene(Scene* scene) {
	m_scene = scene;
}

void DefaultRenderer::SetResolution(const glm::ivec2& resolution) {
	m_resolution = resolution;
	std::vector<Camera>& cameras = m_scene->GetCameras();
	for (size_t i = 0; i < cameras.size(); i++) {
		cameras[i].SetAspect((float)resolution.x / resolution.y);
	}
}

void DefaultRenderer::Reset() {

}

void DefaultRenderer::DrawFrame() {
	if (!m_scene) return;
	Camera* camera = m_scene->GetMainCamera();
	if (!camera) return;

	glUseProgram(m_defaultShader);

	SetupCamera(camera);
	SetupLights();

	GLuint mModelLoc = glGetUniformLocation(m_defaultShader, "mModel");
	SceneObject* rootObject = m_scene->GetRootObject();
	DrawObject(rootObject, mModelLoc);
}

void DefaultRenderer::DrawObject(SceneObject* object, GLuint mModelLoc, Mat4 parentTransform) {
	if (!object) return;
	Mat4 objectTransform = parentTransform * object->transform.GetMatrix();
	if (MeshAnimatorComponent* animatorComponent = object->GetComponent<MeshAnimatorComponent>()) {
		animatorComponent->UpdateAnimation(Timer::deltaTime * 0.25f);
		std::vector<Mat4> transforms = animatorComponent->GetBoneMatrices();
		GLuint mat4Loc = glGetUniformLocation(m_defaultShader, "finalBonesMatrices");
		glUniformMatrix4(mat4Loc, transforms.size(), GL_FALSE, &transforms[0][0][0]);
	}
	if (MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		glUniformMatrix4(mModelLoc, 1, GL_FALSE, &objectTransform[0][0]);
		const Material* material = ResourceManager::GetDefaultMaterial();
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		SetupMaterial(material);
		mesh->Draw();
	}
	for (size_t i = 0; i < object->children.size(); i++) {
		DrawObject(object->children[i], mModelLoc, objectTransform);
	}
}

void DefaultRenderer::SetupCamera(const Camera* camera) {
	GLuint cameraPosLoc = glGetUniformLocation(m_defaultShader, "cameraPos");
	GLuint mViewLoc = glGetUniformLocation(m_defaultShader, "mView");
	GLuint mProjectioLoc = glGetUniformLocation(m_defaultShader, "mProjection");

	glm::fvec3 cameraPos = camera->GetTransform().GetPositionValue();
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4(mViewLoc, 1, GL_FALSE, &camera->GetInverseViewMatrix()[0][0]);
	glUniformMatrix4(mProjectioLoc, 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
}

void DefaultRenderer::SetupLights() {
	const size_t MaxLights = 4;

	GLuint positionsLoc = glGetUniformLocation(m_defaultShader, "lightPositions");
	GLuint colorsLoc = glGetUniformLocation(m_defaultShader, "lightColors");

	GLfloat positions[MaxLights * 3] = { 0 };
	GLfloat colors[MaxLights * 3] = { 0 };

	std::vector<PointLight*>& pointLights = m_scene->GetPointLights();
	for (size_t i = 0; i < pointLights.size() && i < MaxLights; i++) {
		glm::fvec3 center = pointLights[i]->GetTransform().GetPositionValue();
		positions[i * 3 + 0] = center.x;
		positions[i * 3 + 1] = center.y;
		positions[i * 3 + 2] = center.z;
		glm::fvec3 emission = pointLights[i]->Phi().GetRGBValue();
		colors[i * 3 + 0] = emission.x;
		colors[i * 3 + 1] = emission.y;
		colors[i * 3 + 2] = emission.z;
	}

	glUniform3fv(positionsLoc, MaxLights, positions);
	glUniform3fv(colorsLoc, MaxLights, colors);
}

void DefaultRenderer::SetupMaterial(const Material* material) {
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