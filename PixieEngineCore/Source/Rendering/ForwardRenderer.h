#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "EngineTime.h"
#include "Resources/ResourceManager.h"
#include "GlobalRenderer.h"
#include "FrameBuffer.h"
#include "GlobalRenderer.h"

enum class AntiAliasing {
	None = 0,
	SSAAx2,
	SSAAx4,
	SSAAx8,
	MSAAx2,
	MSAAx4,
	MSAAx8,
	MSAAx16,
	FXAA,
	COUNT,
};

std::string to_string(AntiAliasing mode);

class ForwardRenderer {
public:
	ForwardRenderer();

	void DrawFrame(Scene* scene, Camera* camera);
	void SetResolution(glm::ivec2 resolution);
	glm::ivec2 GetFrameResolution() const;
	GLuint GetFrameHandle() const;
	AntiAliasing GetAntiAlisingMode() const;
	void SetAntiAlisingMode(AntiAliasing mode);
	void SetShader(Shader shader);

protected:
	FrameBuffer m_frameBuffer;
	FrameBuffer m_fxaaFrameBuffer;
	MultisampleFrameBuffer m_msFrameBuffer;
	Shader m_defaultShader;
	Shader m_fxaaShader;
	Texture m_LTC1Texture;
	Texture m_LTC2Texture;
	AntiAliasing m_antiAliasing = AntiAliasing::None;
	int32_t m_ssaaScale = 1;
	bool m_useMultisampleFramebuffer = false;
	bool m_useFXAA = false;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
	void SetupCamera(Camera* camera);
	void SetupLights(Scene* scene);
};
