#pragma once
#include "pch.h"
#include "ShaderNode.h"
#include "SSAOKernel.h"
#include "Resources/Shader.h"
#include "Resources/ResourceManager.h"
#include "Resources/TextureGenerator.h"
#include "LTC_Matrix.h"

class DefferedLightingNode : public ShaderNode {
public:
	DefferedLightingNode();

	void Process(const Scene& scene, const Camera& camera) override;

protected:
	const glm::ivec2 SSAONoiseResolution = { 4, 4 };
	Shader m_program;
	GLuint m_frameBuffer = 0;
	Texture m_frame;
	Texture m_noiseTexture;
	Texture m_LTC1Texture;
	Texture m_LTC2Texture;
	SSAOKernel<64> m_ssaoKernel;

	void SetupLights(const Scene& scene);
};