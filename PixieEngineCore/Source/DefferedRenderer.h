#pragma once
#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Components.h"
#include "EngineTime.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "LTC_Matrix.h"

class DefferedRenderer {
public:
	GBuffer m_gBuffer;
	FrameBuffer m_viewportFrameBuffer;

	DefferedRenderer();

	void DrawFrame(Scene* scene, Camera* camera);

protected:
	Shader m_shader;
	Shader m_lightingShader;
	GLuint m_LTC1Texture = 0;
	GLuint m_LTC2Texture = 0;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupMaterial(Material* material);
	void SetupLights(Scene* scene);
	GLuint LoadLTCTexture(const float* matrixTable);
};
