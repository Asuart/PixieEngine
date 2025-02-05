#pragma once
#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Components.h"
#include "EngineTime.h"
#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "LTC_Matrix.h"
#include "TextureGenerator.h"
#include "SSAOKernel.h"
#include "Renderer.h"
#include "GlobalRenderer.h"

class DefferedRenderer {
public:
	GBuffer m_gBuffer;
	PostProcessingFrameBuffer<GL_RED, GL_RED, GL_FLOAT> m_ssaoBuffer;
	PostProcessingFrameBuffer<GL_RED, GL_RED, GL_FLOAT> m_ssaoBlurBuffer;
	FrameBuffer m_frameBuffer;

	DefferedRenderer();

	void DrawFrame(Scene* scene, Camera* camera);

protected:
	const glm::ivec2 SSAONoiseResolution = { 4, 4 };
	Shader m_shader;
	Shader m_ssaoShader;
	Shader m_ssaoBlurShader;
	Shader m_lightingShader;
	Texture m_LTC1Texture;
	Texture m_LTC2Texture;
	Texture m_noiseTexture;
	SSAOKernel<64> m_ssaoKernel;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
	void SetupMaterial(Material* material);
	void SetupLights(Scene* scene);
};
