#pragma once
#include "pch.h"
#include "ShaderNode.h"
#include "Shader.h"
#include "SSAOKernel.h"
#include "ResourceManager.h"
#include "TextureGenerator.h"
#include "LTC_Matrix.h"

class DefferedLightingNode : public ShaderNode {
public:
	DefferedLightingNode();

	void Process(const Scene& scene, const Camera& camera) override;

protected:
	const glm::ivec2 SSAONoiseResolution = { 4, 4 };
	Shader m_program;
	GLuint m_frameBuffer = 0;
	GLuint m_frame = 0;
	GLuint m_noiseTexture = 0;
	GLuint m_LTC1Texture = 0;
	GLuint m_LTC2Texture = 0;
	SSAOKernel<64> m_ssaoKernel;

	void SetupLights(const Scene& scene);
	GLuint LoadLTCTexture(const float* matrixTable);
};