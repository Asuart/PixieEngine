#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "Time/EngineTime.h"
#include "FrameBuffer.h"
#include "LTC_Matrix.h"
#include "SSAOKernel.h"
#include "Renderer.h"
#include "RenderEngine.h"

namespace PixieEngine {

class DefferedRenderer : public Renderer {
public:
	DefferedRenderer();

	void DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const override;
	void SetResolution(glm::ivec2 resolution) override;
	glm::ivec2 GetResolution() const override;
	GLuint GetFrameHandle() const override;
	AntiAliasing GetAntiAliasing() const override;
	void SetAntiAliasing(AntiAliasing mode) override;

protected:
	const glm::ivec2 SSAONoiseResolution = { 4, 4 };
	GBuffer m_gBuffer;
	PostProcessingFrameBuffer<GL_RED, GL_RED, GL_FLOAT> m_ssaoBuffer;
	PostProcessingFrameBuffer<GL_RED, GL_RED, GL_FLOAT> m_ssaoBlurBuffer;
	FrameBuffer m_frameBuffer;
	Shader m_shader;
	Shader m_ssaoShader;
	Shader m_ssaoBlurShader;
	Shader m_lightingShader;
	Texture m_LTC1Texture;
	Texture m_LTC2Texture;
	Texture m_noiseTexture;
	SSAOKernel<64> m_ssaoKernel;

	void DrawObject(SceneObject* object, glm::mat4 parentTransform = glm::mat4(1.0f)) const;
	void SetupMaterial(std::shared_ptr<Material> material) const;
	void SetupLights(std::shared_ptr<Scene> scene) const;
};

}