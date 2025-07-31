#include "pch.h"
#include "RenderEngine.h"
#include "Resources/MeshGenerator.h"
#include "Log.h"

inline void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// filter warnings
	if (id == 131185 || id == 131218 || id == 131186) return;

	std::cout << "---------------------opengl-callback-start------------\n";
	std::cout << "message: " << message << "\n";
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	default:
		std::cout << "UNDEFINED";
	}
	std::cout << "\n";

	std::cout << "id: " << id << "\n";
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	default:
		std::cout << "UNDEFINED";
	}
	std::cout << "\n---------------------opengl-callback-end--------------\n";
}

Shader RenderEngine::m_quadShader;
Shader RenderEngine::m_textShader;
Shader RenderEngine::m_uiBoxShader;
Shader RenderEngine::m_skyboxShader;
Shader RenderEngine::m_equirectangularToCubemapShader;
Shader RenderEngine::m_cubemapConvolutionShader;
Shader RenderEngine::m_prefilterShader;
Shader RenderEngine::m_brdfLUTShader;
GLuint RenderEngine::m_textVAO;
GLuint RenderEngine::m_textVBO;
Texture* RenderEngine::m_brdfLUT;
MeshHandle* RenderEngine::m_quadMesh;
MeshHandle* RenderEngine::m_cubeMesh;
MeshHandle* RenderEngine::m_sphereMesh;
std::map<char, FontCharacter> RenderEngine::m_characters;
uint32_t RenderEngine::m_defaultFontSize;

void RenderEngine::DrawMesh(const MeshHandle& mesh) {
	if (mesh.m_vao == 0 || mesh.m_indicesCount == 0) return;
	glBindVertexArray(mesh.m_vao);
	glDrawElements(GL_TRIANGLES, mesh.m_indicesCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void RenderEngine::DrawMeshWireframe(const MeshHandle& mesh) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMesh(mesh);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool RenderEngine::Initialize() {
	if (!gladLoadGL()) {
		Log::Error("GLAD initialization failed");
		exit(2);
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(openglCallbackFunction, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	m_quadShader = ShaderManager::LoadShader("TextureViewerQuad");
	m_textShader = ShaderManager::LoadShader("Text");
	m_uiBoxShader = ShaderManager::LoadShader("UIBox");
	m_skyboxShader = ShaderManager::LoadShader("Skybox");
	m_cubemapConvolutionShader = ShaderManager::LoadShader("CubemapConvolution");
	m_equirectangularToCubemapShader = ShaderManager::LoadShader("EquirectangularToCubemap");
	m_prefilterShader = ShaderManager::LoadShader("Prefilter");
	m_brdfLUTShader = ShaderManager::LoadShader("BRDFLookUpTexture");

	m_quadMesh = new MeshHandle(MeshGenerator::Quad({ 0.0f, 0.0f }, { 1.0f, 1.0f }));
	m_cubeMesh = new MeshHandle(MeshGenerator::Cube());
	m_sphereMesh = new MeshHandle(MeshGenerator::SphereFromOctahedron(1.0f, 6));

	glGenVertexArrays(1, &m_textVAO);
	glGenBuffers(1, &m_textVBO);
	glBindVertexArray(m_textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_brdfLUT = new Texture({ 512, 512 }, GL_RG16F, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear);
	DrawBRDFLookUpTexture({ 512, 512 }, m_brdfLUT->GetHandle());

	m_defaultFontSize = 64;
	m_characters = FontLoader::LoadDefaultFont(m_defaultFontSize);

	return true;
}

void RenderEngine::Free() {
	m_quadShader.Free();
	m_textShader.Free();
	m_uiBoxShader.Free();
	m_skyboxShader.Free();
	m_equirectangularToCubemapShader.Free();
	m_cubemapConvolutionShader.Free();
	m_prefilterShader.Free();
	m_brdfLUTShader.Free();

	delete m_brdfLUT;
	delete m_quadMesh;
	delete m_cubeMesh;
	delete m_sphereMesh;

	glDeleteVertexArrays(1, &m_textVAO);
	m_textVAO = 0;
	glDeleteBuffers(1, &m_textVBO);
	m_textVBO = 0;

	m_characters.clear();
}

void RenderEngine::DrawSkybox(const Camera& camera, GLuint skyboxTexture) {
	glDepthFunc(GL_LEQUAL);
	m_skyboxShader.Bind();
	m_skyboxShader.SetUniformMat4f("mProjection", camera.GetProjectionMatrix());
	m_skyboxShader.SetUniformMat4f("mView", camera.GetViewMatrix());
	m_skyboxShader.SetCubeMap("environmentMap", skyboxTexture, 0);
	DrawCube();
}

void RenderEngine::DrawQuad() {
	DrawMesh(*m_quadMesh);
}

void RenderEngine::DrawCube() {
	DrawMesh(*m_cubeMesh);
}

void RenderEngine::DrawSphere() {
	DrawMesh(*m_sphereMesh);
}

void RenderEngine::DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution) {
	DrawAccumulatorTextureFitted(id, 1, textureResolution, viewportResolution);
}

void RenderEngine::DrawAccumulatorTextureFitted(GLuint id, int32_t samples, glm::ivec2 textureResolution, glm::ivec2 viewportResolution) {
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
	DrawAccumulatorTexture(id, samples, pos, size);
}

void RenderEngine::DrawTexture(GLuint id, glm::vec2 pos, glm::vec2 size) {
	DrawAccumulatorTexture(id, 1, pos, size);
}

void RenderEngine::DrawAccumulatorTexture(GLuint id, int32_t samples, glm::vec2 pos, glm::vec2 size) {
	m_quadShader.Bind();
	m_quadShader.SetUniform2f("uPos", pos);
	m_quadShader.SetUniform2f("uSize", size);
	m_quadShader.SetUniform1f("uSamples", (float)samples);
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

void RenderEngine::DrawUIBox(glm::vec2 position, glm::vec2 size, glm::vec4 baseColor, float borderRadius, float borderWidth, glm::vec4 borderColor, glm::mat4 projection) {
	m_uiBoxShader.Bind();
	m_uiBoxShader.SetUniform2f("uPos", position);
	m_uiBoxShader.SetUniform2f("uSize", size);
	m_uiBoxShader.SetUniform2i("uBoxPixelSize", glm::ivec2(size));
	m_uiBoxShader.SetUniform1f("uBorderRadius", borderRadius);
	m_uiBoxShader.SetUniform4f("uBaseColor", baseColor);
	m_uiBoxShader.SetUniformMat4f("mProjection", projection);
	DrawQuad();
	m_uiBoxShader.Unbind();
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

GLuint RenderEngine::GetBRDFLUTHandle() {
	return m_brdfLUT->GetHandle();
}
