#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(const glm::ivec2& resolution)
	: Renderer(resolution) {
	defaultShader = CompileShader(DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE);
}

void SceneRenderer::SetScene(Scene* scene) {
	m_scene = scene;
}

void SceneRenderer::DrawFrame() {
	glUseProgram(defaultShader);
	SceneObject* rootObject = m_scene->GetRootObject();
	DrawObject(rootObject);
}

void SceneRenderer::DrawObject(SceneObject* object) {
	if (MeshComponent* mesh = object->GetComponent<MeshComponent>()) {
		mesh->Draw();
	}
	for (size_t i = 0; i < object->children.size(); i++) {
		DrawObject(object->children[i]);
	}
}