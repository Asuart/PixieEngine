#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(const glm::ivec2& resolution, Scene* scene)
	: Renderer(resolution), m_scene(scene) {
	m_defaultShader = CompileShader(DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE);
}

void SceneRenderer::SetScene(Scene* scene) {
	scene = scene;
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

	GLuint mModelLoc = glGetUniformLocation(m_defaultShader, "mModel");
	GLuint mViewLoc = glGetUniformLocation(m_defaultShader, "mView");
	GLuint mProjectioLoc = glGetUniformLocation(m_defaultShader, "mProjection");
	
	glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &camera->GetInverseViewMatrix()[0][0]);
	glUniformMatrix4fv(mProjectioLoc, 1, GL_FALSE, &camera->GetProjectionMatrix()[0][0]);

	SceneObject* rootObject = m_scene->GetRootObject();
	DrawObject(rootObject, mModelLoc);
}

void SceneRenderer::DrawObject(SceneObject* object, GLuint mModelLoc) {
	if (!object) return;
	if (MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &object->transform.GetMatrix()[0][0]);
		mesh->Draw();
	}
	for (size_t i = 0; i < object->children.size(); i++) {
		DrawObject(object->children[i], mModelLoc);
	}
}