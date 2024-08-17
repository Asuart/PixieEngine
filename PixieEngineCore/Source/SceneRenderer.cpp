#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(const glm::ivec2& resolution, Scene* scene)
	: Renderer(resolution), m_scene(scene) {
	m_defaultShader = CompileShader(PBR_VERTEX_SHADER_SOURCE, PBR_FRAGMENT_SHADER_SOURCE);
}

void SceneRenderer::SetScene(Scene* scene) {
	m_scene = scene;
}

void SceneRenderer::SetResolution(const glm::ivec2& resolution) {
	m_resolution = resolution;
	std::vector<Camera>& cameras = m_scene->GetCameras();
	for (size_t i = 0; i < cameras.size(); i++) {
		cameras[i].SetAspect((float)resolution.x / resolution.y);
	}
}

void SceneRenderer::Reset() {

}

void SceneRenderer::DrawFrame() {
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

void SceneRenderer::DrawObject(SceneObject* object, GLuint mModelLoc) {
	if (!object) return;
	if (MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &object->transform.GetMatrix()[0][0]);
		const Material* material = m_scene->GetMaterialsList()[0];
		if (MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>()) {
			material = materialComponent->GetMaterial();
		}
		SetupMaterial(material);
		mesh->Draw();
	}
	for (size_t i = 0; i < object->children.size(); i++) {
		DrawObject(object->children[i], mModelLoc);
	}
}

void SceneRenderer::SetupCamera(const Camera* camera) {
	GLuint cameraPosLoc = glGetUniformLocation(m_defaultShader, "cameraPos");
	GLuint mViewLoc = glGetUniformLocation(m_defaultShader, "mView");
	GLuint mProjectioLoc = glGetUniformLocation(m_defaultShader, "mProjection");

	const glm::fvec3& cameraPos = camera->m_transform.GetPositionValue();
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &camera->GetInverseViewMatrix()[0][0]);
	glUniformMatrix4fv(mProjectioLoc, 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);
}

void SceneRenderer::SetupLights() {
	const size_t MaxLights = 4;

	GLuint positionsLoc = glGetUniformLocation(m_defaultShader, "lightPositions");
	GLuint colorsLoc = glGetUniformLocation(m_defaultShader, "lightColors");

	GLfloat positions[MaxLights * 3] = { 0 };
	GLfloat colors[MaxLights * 3] = { 0 };

	std::vector<MaterialComponent*>& areaLights = m_scene->GetAreaLights();
	for (size_t i = 0; i < areaLights.size() && i < MaxLights; i++) {
		const Mesh* mesh = areaLights[i]->parent->GetComponent<MeshComponent>()->GetMesh();
		const glm::fvec3& center = mesh->GetCenter() + areaLights[i]->parent->transform.GetPositionValue();
		positions[i * 3 + 0] = center.x;
		positions[i * 3 + 1] = center.y;
		positions[i * 3 + 2] = center.z;
		const glm::fvec3& emission = areaLights[i]->GetMaterial()->m_emission.GetRGBValue();
		colors[i * 3 + 0] = emission.x;
		colors[i * 3 + 1] = emission.y;
		colors[i * 3 + 2] = emission.z;
	}

	glUniform3fv(positionsLoc, MaxLights, positions);
	glUniform3fv(colorsLoc, MaxLights, colors);
}

void SceneRenderer::SetupMaterial(const Material* material) {
	GLuint albedoLoc = glGetUniformLocation(m_defaultShader, "albedo");
	GLuint metallicLoc = glGetUniformLocation(m_defaultShader, "metallic");
	GLuint roghnessLoc = glGetUniformLocation(m_defaultShader, "roughness");
	GLuint ambientOcclusionLoc = glGetUniformLocation(m_defaultShader, "ambientOcclusion");

	glm::fvec3 albedo = material->m_albedo.GetRGBValue();
	glUniform3f(albedoLoc, albedo.x, albedo.y, albedo.z);
	glUniform1f(metallicLoc, material->m_metallic);
	glUniform1f(roghnessLoc, material->m_roughness);
	glUniform1f(ambientOcclusionLoc, material->m_ambiendOcclusion);
}