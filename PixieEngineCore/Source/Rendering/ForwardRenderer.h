#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "EngineTime.h"
#include "RenderEngine.h"
#include "FrameBuffer.h"
#include "Renderer.h"

class ForwardRenderer : public Renderer {
public:
	ForwardRenderer();

	void DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const override;
	void SetResolution(glm::ivec2 resolution) override;
	glm::ivec2 GetResolution() const override;
	GLuint GetFrameHandle() const override;
	AntiAliasing GetAntiAliasing() const override;
	void SetAntiAliasing(AntiAliasing mode) override;

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

	void DrawObject(SceneObject* object, glm::mat4 parentTransform = glm::mat4(1.0f)) const;
	void SetupCamera(const Camera& camera) const;
	void SetupLights(std::shared_ptr<Scene> scene) const;
};
