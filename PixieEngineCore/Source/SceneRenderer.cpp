#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(const glm::ivec2& resolution, Scene* scene)
	: Renderer(resolution), scene(scene) {
	defaultShader = CompileShader(DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE);
}

void SceneRenderer::SetScene(Scene* scene) {
	scene = scene;
}

void SceneRenderer::Reset() {

}

void SceneRenderer::DrawFrame() {
	if (!scene) return;
	Camera* camera = scene->GetMainCamera();
	if (!camera) return;

	glUseProgram(defaultShader);

	GLuint mModelLoc = glGetUniformLocation(defaultShader, "mModel");
	GLuint mViewLoc = glGetUniformLocation(defaultShader, "mView");
	GLuint mProjectioLoc = glGetUniformLocation(defaultShader, "mProjection");
	
	//glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &camera->transform.GetMatrix()[0][0]);
	const glm::mat4& projection = camera->GetProjectionMatrix();
	const glm::mat4& view = camera->GetViewMatrix();;
	glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(mProjectioLoc, 1, GL_FALSE, &projection[0][0]);

	SceneObject* rootObject = scene->GetRootObject();
	DrawObject(rootObject, mModelLoc);
}

void SceneRenderer::DrawObject(SceneObject* object, GLuint mModelLoc) {
	if (MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, &object->transform.GetMatrix()[0][0]);
		mesh->Draw();
	}
	for (size_t i = 0; i < object->children.size(); i++) {
		DrawObject(object->children[i], mModelLoc);
	}
}