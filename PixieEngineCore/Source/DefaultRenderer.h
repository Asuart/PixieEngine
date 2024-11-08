#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "MeshAnimatorComponent.h"
#include "ShaderLibrary.h"
#include "Light.h"
#include "Timer.h"
#include "ResourceManager.h"

class DefaultRenderer : public Renderer {
public:
	DefaultRenderer();

	void DrawFrame(Scene* scene, Camera* camera) override;

protected:
	GLuint m_defaultShader = 0;

	void DrawObject(SceneObject* object, GLuint mModelLoc, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupLights(Scene* scene);
	void SetupMaterial(Material* material);
};
