#pragma once
#include "pch.h"
#include "Rendering/RendererBase.h"

namespace PixieEngine {

struct MeshDataOpenGL {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	uint32_t indicesCount = 0;
};

class RendererOpenGL : public RendererBase {
public:
	RendererOpenGL();
	~RendererOpenGL();

	void StartFrame() override;
	void EndFrame() override;
	void DrawMesh(MeshHandle mesh) override;

	std::vector<GLuint> m_shaders;
	std::vector<MeshDataOpenGL> m_meshes;

	Shader m_quadShader;
	Shader m_textShader;
	Shader m_uiBoxShader;
	Shader m_equirectangularToCubemapShader;
	Shader m_cubemapConvolutionShader;
	Shader m_prefilterShader;
	Shader m_brdfLUTShader;
	Shader m_skyboxShader;
	GLuint m_textVAO;
	GLuint m_textVBO;
	Texture* m_brdfLUT;
	MeshHandle m_quadMesh;
	MeshHandle m_cubeMesh;
	MeshHandle m_sphereMesh;
	std::map<char, FontCharacter> m_characters;
	uint32_t m_defaultFontSize;
};

}