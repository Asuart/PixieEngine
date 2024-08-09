#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "ShaderLibrary.h"

class SceneRenderer : public Renderer {
public:
	SceneRenderer(const glm::ivec2& resolution, Scene* scene = nullptr);

	void SetScene(Scene* scene);
	void SetResolution(const glm::ivec2& resolution) override;
	void DrawFrame();
	void Reset() override;

protected:
	Scene* m_scene = nullptr;
	GLuint m_defaultShader = 0;

	void DrawObject(SceneObject* object, GLuint mModelLoc);
};