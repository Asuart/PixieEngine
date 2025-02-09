#include "pch.h"
#include "GlobalRenderer.h"

Shader GlobalRenderer::m_quadShader;
Shader GlobalRenderer::m_textShader;
Shader GlobalRenderer::m_uiBoxShader;
Shader GlobalRenderer::m_skyboxShader;
Shader GlobalRenderer::m_equirectangularToCubemapShader;
Shader GlobalRenderer::m_cubemapConvolutionShader;
Shader GlobalRenderer::m_prefilterShader;
Shader GlobalRenderer::m_brdfLUTShader;
GLuint GlobalRenderer::m_textVAO;
GLuint GlobalRenderer::m_textVBO;

void GlobalRenderer::DrawMesh(Mesh* mesh) {
	if (!mesh->m_vao) return;
	glBindVertexArray(mesh->m_vao);
	glDrawElements(GL_TRIANGLES, mesh->m_indicesCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void GlobalRenderer::DrawMeshWireframe(Mesh* mesh) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMesh(mesh);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GlobalRenderer::Initialize() {
	m_quadShader = ResourceManager::LoadShader("TextureViewerQuad");
	m_textShader = ResourceManager::LoadShader("Text");
	m_uiBoxShader = ResourceManager::LoadShader("UIBox");
	m_skyboxShader = ResourceManager::LoadShader("Skybox");
	m_cubemapConvolutionShader = ResourceManager::LoadShader("CubemapConvolution");
	m_equirectangularToCubemapShader = ResourceManager::LoadShader("EquirectangularToCubemap");
	m_prefilterShader = ResourceManager::LoadShader("Prefilter");
	m_brdfLUTShader = ResourceManager::LoadShader("BRDFLookUpTexture");

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

void GlobalRenderer::DrawSkybox(const Camera& camera, GLuint skyboxTexture) {
	glDepthFunc(GL_LEQUAL);
	m_skyboxShader.Bind();
	m_skyboxShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	m_skyboxShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_skyboxShader.SetCubeMap("environmentMap", skyboxTexture, 0);
	DrawMesh(ResourceManager::GetCubeMesh());
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
	DrawMesh(ResourceManager::GetQuadMesh());
}

void GlobalRenderer::DrawText(const std::string& text, Vec2 position, Float fontSize, Vec3 color, Mat4 projection, bool rightToLeft) {
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

		DrawMesh(ResourceManager::GetQuadMesh());

		position.x += (ch.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GlobalRenderer::DrawUIBox(Vec2 position, Vec2 size, Vec4 baseColor, Float borderRadius, Float borderWidth, Vec4 borderColor, Mat4 projection) {
	m_uiBoxShader.Bind();
	m_uiBoxShader.SetUniform2f("uPos", position);
	m_uiBoxShader.SetUniform2f("uSize", size);
	m_uiBoxShader.SetUniform2i("uBoxPixelSize", glm::ivec2(size));
	m_uiBoxShader.SetUniform1f("uBorderRadius", borderRadius);
	m_uiBoxShader.SetUniform4f("uBaseColor", baseColor);
	m_uiBoxShader.SetUniformMat4f("mProjection", projection);
	DrawMesh(ResourceManager::GetQuadMesh());
	m_uiBoxShader.Unbind();
}

void GlobalRenderer::DrawCubeMap(GLuint equirectangularTexture, glm::ivec2 cubemapResolution, GLuint cubemapTexture, glm::ivec2 lighmapResolution, GLuint lightmapTextrue, glm::ivec2 prefilterResolution, GLuint prefilterTexture) {
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

	GLuint captureFBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	GLuint captureRBO;
	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapResolution.x, cubemapResolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glViewport(0, 0, cubemapResolution.x, cubemapResolution.y);

	m_equirectangularToCubemapShader.Bind();
	m_equirectangularToCubemapShader.SetUniform1i("equirectangularMap", 0);
	m_equirectangularToCubemapShader.SetUniformMat4f("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangularTexture);
	for (int32_t i = 0; i < 6; i++) {
		m_equirectangularToCubemapShader.SetUniformMat4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTexture, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawMesh(ResourceManager::GetCubeMesh());
	}

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, lighmapResolution.x, lighmapResolution.y);
	glViewport(0, 0, lighmapResolution.x, lighmapResolution.y);

	m_cubemapConvolutionShader.Bind();
	m_cubemapConvolutionShader.SetUniform1i("environmentMap", 0);
	m_cubemapConvolutionShader.SetUniformMat4f("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	for (int32_t i = 0; i < 6; i++) {
		m_cubemapConvolutionShader.SetUniformMat4f("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lightmapTextrue, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawMesh(ResourceManager::GetCubeMesh());
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterTexture);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	m_prefilterShader.Bind();
	m_prefilterShader.SetUniform1i("environmentMap", 0);
	m_prefilterShader.SetUniformMat4f("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	uint32_t maxMipLevels = 5;
	for (uint32_t mip = 0; mip < maxMipLevels; mip++) {
		uint32_t mipWidth = glm::max(1, cubemapResolution.x >> mip);
		uint32_t mipHeight = glm::max(1, cubemapResolution.y >> mip);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		Float roughness = (Float)mip / (Float)(maxMipLevels - 1);
		m_prefilterShader.SetUniform1f("roughness", roughness);
		for (int32_t i = 0; i < 6; i++) {
			m_prefilterShader.SetUniformMat4f("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterTexture, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			DrawMesh(ResourceManager::GetCubeMesh());
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void GlobalRenderer::DrawBRDFLookUpTexture(glm::ivec2 resolution, GLuint texture) {
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
	DrawMesh(ResourceManager::GetQuadMesh());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteFramebuffers(1, &captureFBO);

	// Restore original viewport
	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}
