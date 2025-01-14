#pragma once
#include "pch.h"
#include "ResourceManager.h"

class GlobalRenderer {
public:
	static void Initialize();

	// Draw texture fitting it to screen
	static void DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);
	static void DrawAccumulatorTextureFitted(GLuint id, int32_t samples, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);

	// Draws texture at size and coords set in screen space
	static void DrawTexture(GLuint id, Vec2 pos, Vec2 size);
	static void DrawAccumulatorTexture(GLuint id, int32_t samples, Vec2 pos, Vec2 size);

protected:
	static Shader m_quadShader;
};