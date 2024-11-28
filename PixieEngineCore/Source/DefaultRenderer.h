#pragma once
#include "Scene.h"
#include "Camera.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "MeshAnimatorComponent.h"
#include "Lights.h"
#include "EngineTime.h"
#include "ResourceManager.h"

class DefaultRenderer {
public:
	DefaultRenderer();

	void DrawFrame(Scene* scene, Camera* camera);
	void DrawTexture(GLuint texture, glm::ivec2 textureResolution, glm::ivec2 viewportResolution, int32_t samples = 1);

protected:
	GLuint m_defaultShader = 0;
	GLuint m_quadShader = 0;

	void DrawObject(SceneObject* object, GLuint mModelLoc, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupLights(Scene* scene);
	void SetupMaterial(Material* material);
};
