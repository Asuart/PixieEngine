#include "pch.h"
#include "GlobalRenderer.h"

Shader GlobalRenderer::m_quadShader;

void GlobalRenderer::Initialize() {
	m_quadShader = ResourceManager::LoadShader("TextureViewerQuadVertexShader.glsl", "TextureViewerQuadFragmentShader.glsl");
}

void GlobalRenderer::DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution) {
	DrawAccumulatorTextureFitted(id, 1, textureResolution, viewportResolution);
}

void GlobalRenderer::DrawAccumulatorTextureFitted(GLuint id, int32_t samples, glm::ivec2 textureResolution, glm::ivec2 viewportResolution) {
	Vec2 pos(0.0f, 0.0f), size(1.0f, 1.0f);
	Float textureAspect = Aspect(textureResolution);
	Float viewportAspect = Aspect(viewportResolution);
	if (viewportAspect > textureAspect) {
		size.x = textureAspect / viewportAspect;
		pos.x = (1.0f - size.x) * 0.5f;
	}
	else {
		size.y = viewportAspect / textureAspect;
		pos.y = (1.0f - size.y) * 0.5f;
	}
	DrawAccumulatorTexture(id, samples, pos, size);
}

void GlobalRenderer::DrawTexture(GLuint id, Vec2 pos, Vec2 size) {
	DrawAccumulatorTexture(id, 1, pos, size);
}

void GlobalRenderer::DrawAccumulatorTexture(GLuint id, int32_t samples, Vec2 pos, Vec2 size) {
	m_quadShader.Bind();
	m_quadShader.SetUniform2f("uPos", pos);
	m_quadShader.SetUniform2f("uSize", size);
	m_quadShader.SetUniform1f("uSamples", (Float)samples);
	m_quadShader.SetTexture("ourTexture", id, 2);
	ResourceManager::GetQuadMesh()->Draw();
}