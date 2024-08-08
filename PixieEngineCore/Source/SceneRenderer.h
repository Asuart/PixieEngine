#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "ShaderLibrary.h"

class SceneRenderer : public Renderer {
public:
	SceneRenderer(const glm::ivec2& resolution);

	void SetScene(Scene* scene);
	void DrawFrame();

protected:
	Scene* m_scene;
	GLuint defaultShader;

	void DrawObject(SceneObject* object);
};