#include "pch.h"
#include "GlobalRenderer.h"

Shader GlobalRenderer::m_quadShader;
Shader GlobalRenderer::m_textShader;
Shader GlobalRenderer::m_uiBoxShader;
GLuint GlobalRenderer::m_textVAO;
GLuint GlobalRenderer::m_textVBO;

void GlobalRenderer::Initialize() {
	m_quadShader = ResourceManager::LoadShader("TextureViewerQuadVertexShader.glsl", "TextureViewerQuadFragmentShader.glsl");
	m_textShader = ResourceManager::LoadShader("TextVertexShader.glsl", "TextFragmentShader.glsl");
	m_uiBoxShader = ResourceManager::LoadShader("UIBoxVertexShader.glsl", "UIBoxFragmentShader.glsl");

	glGenVertexArrays(1, &m_textVAO);
	glGenBuffers(1, &m_textVBO);
	glBindVertexArray(m_textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

void GlobalRenderer::DrawText(const std::string& text, Vec2 position, Float fontSize, Vec3 color, Mat4 projection, bool rightToLeft) {
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	m_textShader.Bind();
	m_textShader.SetUniform3f("textColor", color);
	m_textShader.SetUniformMat4f("mProjection", projection);

	Float scale = fontSize / ResourceManager::GetDefaultFontSize();
	Float xOffset = 0;
	if (rightToLeft) {
		for (int32_t i = 0; i < text.size(); i++) {
			const FontCharacter& ch = ResourceManager::GetFontCharacter(text[i]);
			xOffset += (ch.advance >> 6) * scale;
		}
	}

	for (int32_t i = 0; i < text.size(); i++) {
		const FontCharacter& ch = ResourceManager::GetFontCharacter(text[i]);

		Float xpos = position.x + ch.bearing.x * scale - xOffset;
		Float ypos = position.y - ch.bearing.y * scale;
		Float w = ch.size.x * scale;
		Float h = ch.size.y * scale;

		m_textShader.SetUniform2f("uPos", Vec2(xpos, ypos));
		m_textShader.SetUniform2f("uSize", Vec2(ch.size) * scale);

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		ResourceManager::GetQuadMesh()->Draw();

		position.x += (ch.advance >> 6) * scale;
	}


	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void GlobalRenderer::DrawUIBox(Vec2 position, Vec2 size, Vec4 baseColor, Float borderRadius, Float borderWidth, Vec4 borderColor, Mat4 projection) {
	m_uiBoxShader.Bind();
	m_uiBoxShader.SetUniform2f("uPos", position);
	m_uiBoxShader.SetUniform2f("uSize", size);
	m_uiBoxShader.SetUniform2i("uBoxPixelSize", glm::ivec2(size));
	m_uiBoxShader.SetUniform1f("uBorderRadius", borderRadius);
	m_uiBoxShader.SetUniform4f("uBaseColor", baseColor);
	m_uiBoxShader.SetUniformMat4f("mProjection", projection);
	ResourceManager::GetQuadMesh()->Draw();
	m_uiBoxShader.Unbind();
}
