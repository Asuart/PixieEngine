#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "EngineTime.h"
#include "Resources/ResourceManager.h"
#include "GlobalRenderer.h"
#include "FrameBuffer.h"
#include "GlobalRenderer.h"

class ForwardRenderer {
public:
	FrameBuffer m_frameBuffer;

	ForwardRenderer();

	void DrawFrame(Scene* scene, Camera* camera);

protected:
	Shader m_defaultShader;
	Texture m_LTC1Texture;
	Texture m_LTC2Texture;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupLights(Scene* scene);
};
