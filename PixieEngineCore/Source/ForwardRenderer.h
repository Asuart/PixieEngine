#pragma once
#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Components.h"
#include "EngineTime.h"
#include "ResourceManager.h"

class ForwardRenderer {
public:
	ForwardRenderer();

	void DrawFrame(Scene* scene, Camera* camera);
	void DrawTexture(GLuint texture, glm::ivec2 textureResolution, glm::ivec2 viewportResolution, int32_t samples = 1);

protected:
	Shader m_defaultShader;
	Shader m_quadShader;
	GLuint m_LTC1Texture = 0;
	GLuint m_LTC2Texture = 0;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupLights(Scene* scene);
	void SetupMaterial(Material* material);
	GLuint LoadLTCTexture(const float* matrixTable);
};
