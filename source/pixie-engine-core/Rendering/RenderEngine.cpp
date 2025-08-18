#include "pch.h"
#include "RenderEngine.h"
#include "Resources/MeshGenerator.h"
#include "Debug/Log.h"
#include "Debug/OpenGLCallbacks.h"
#include "Rendering/OpenGL/RendererOpenGL.h"
#include "Rendering/Vulkan/RendererVulkan.h"

namespace PixieEngine {

RenderAPI RenderEngine::GetRenderAPI() {
	return s_renderAPI;
}

RendererBase* RenderEngine::GetRenderer() {
	return s_renderer;
}

void RenderEngine::DrawMesh(MeshHandle mesh) {
	s_renderer->DrawMesh(mesh);
}

bool RenderEngine::Initialize(RenderAPI api) {
	s_renderAPI = api;
	if (api == RenderAPI::Vulkan) {
		s_renderer = new RendererVulkan();
	}
	else if (api == RenderAPI::OpenGL) {
		s_renderer = new RendererOpenGL();
	}
	else {
		Log::Error("Failed to initialize render engine. Unhandled render API.");
		return false;
	}

	return true;
}

void RenderEngine::Free() {
	delete s_renderer;
	s_renderer = nullptr;
}

void RenderEngine::DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution) {
	glm::vec2 pos(0.0f, 0.0f), size(1.0f, 1.0f);
	float textureAspect = Aspect(textureResolution);
	float viewportAspect = Aspect(viewportResolution);
	if (viewportAspect > textureAspect) {
		size.x = textureAspect / viewportAspect;
		pos.x = (1.0f - size.x) * 0.5f;
	}
	else {
		size.y = viewportAspect / textureAspect;
		pos.y = (1.0f - size.y) * 0.5f;
	}
	DrawTexture(id, pos, size);
}

void RenderEngine::DrawTexture(GLuint id, glm::vec2 pos, glm::vec2 size) {
	m_quadShader.Bind();
	m_quadShader.SetUniform2f("uPos", pos);
	m_quadShader.SetUniform2f("uSize", size);
	m_quadShader.SetTexture("ourTexture", id, 2);
	DrawQuad();
}

void RenderEngine::DrawText(const std::string& text, glm::vec2 position, float fontSize, glm::vec3 color, glm::mat4 projection, bool rightToLeft) {
	glActiveTexture(GL_TEXTURE0);

	m_textShader.Bind();
	m_textShader.SetUniform3f("textColor", color);
	m_textShader.SetUniformMat4f("mProjection", projection);

	float scale = fontSize / m_defaultFontSize;
	float xOffset = 0;
	if (rightToLeft) {
		for (int32_t i = 0; i < text.size(); i++) {
			const FontCharacter& ch = m_characters[text[i]];
			xOffset += (ch.advance >> 6) * scale;
		}
	}

	for (int32_t i = 0; i < text.size(); i++) {
		const FontCharacter& ch = m_characters[text[i]];

		float xpos = position.x + ch.bearing.x * scale - xOffset;
		float ypos = position.y - ch.bearing.y * scale;
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		m_textShader.SetUniform2f("uPos", glm::vec2(xpos, ypos));
		m_textShader.SetUniform2f("uSize", glm::vec2(ch.size) * scale);

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		DrawQuad();

		position.x += (ch.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderEngine::DrawCubeMap(const Texture& equirectangularTexture, glm::ivec2 cubemapResolution, const Cubemap& cubemapTexture, glm::ivec2 lighmapResolution, const Cubemap& lightmapTextrue, glm::ivec2 prefilterResolution, const Cubemap& prefilterTexture) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = {
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, cubemapResolution.x, cubemapResolution.y);

	m_equirectangularToCubemapShader.Bind();
	m_equirectangularToCubemapShader.SetUniform1i("equirectangularMap", 0);
	m_equirectangularToCubemapShader.SetUniformMat4f("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangularTexture.GetHandle());
	for (int32_t i = 0; i < 6; i++) {
		m_equirectangularToCubemapShader.SetUniformMat4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTexture.GetHandle(), 0);
		glClear(GL_COLOR_BUFFER_BIT);
		DrawCube();
	}

	glViewport(0, 0, lighmapResolution.x, lighmapResolution.y);

	m_cubemapConvolutionShader.Bind();
	m_cubemapConvolutionShader.SetUniform1i("environmentMap", 0);
	m_cubemapConvolutionShader.SetUniformMat4f("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture.GetHandle());
	for (int32_t i = 0; i < 6; i++) {
		m_cubemapConvolutionShader.SetUniformMat4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lightmapTextrue.GetHandle(), 0);
		glClear(GL_COLOR_BUFFER_BIT);
		DrawCube();
	}

	m_prefilterShader.Bind();
	m_prefilterShader.SetUniform1i("environmentMap", 0);
	m_prefilterShader.SetUniformMat4f("projection", captureProjection);

	prefilterTexture.GenerateMipmaps();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture.GetHandle());
	uint32_t maxMipLevels = 5;
	for (uint32_t mip = 0; mip < maxMipLevels; mip++) {
		uint32_t mipWidth = glm::max(1, cubemapResolution.x >> mip);
		uint32_t mipHeight = glm::max(1, cubemapResolution.y >> mip);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		m_prefilterShader.SetUniform1f("roughness", roughness);
		for (int32_t i = 0; i < 6; i++) {
			m_prefilterShader.SetUniformMat4f("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterTexture.GetHandle(), mip);
			glClear(GL_COLOR_BUFFER_BIT);
			DrawCube();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &frameBuffer);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void RenderEngine::DrawBRDFLookUpTexture(glm::ivec2 resolution, GLuint texture) {
	// Store Original Viewport
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	GLuint captureFBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	GLuint captureRBO;
	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution.x, resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glViewport(0, 0, resolution.x, resolution.y);
	m_brdfLUTShader.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

}