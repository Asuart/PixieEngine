#include "pch.h"
#include "RendererOpenGL.h"
#include "Debug/Log.h"

namespace PixieEngine {

RendererOpenGL::RendererOpenGL() {
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
}

RendererOpenGL::~RendererOpenGL() {
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

void RendererOpenGL::StartFrame() {

}

void RendererOpenGL::EndFrame() {

}

void RendererOpenGL::DrawMesh(MeshHandle mesh) {
	const MeshDataOpenGL& meshData = m_meshes[mesh.id];
	glBindVertexArray(meshData.vao);
	glDrawElements(GL_TRIANGLES, meshData.indicesCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

}