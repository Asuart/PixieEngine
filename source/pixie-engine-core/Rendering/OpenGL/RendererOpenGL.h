#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Rendering/RendererBase.h"
#include "MeshOpenGL.h"
#include "TextureOpenGL.h"
#include "Utils/Buffer2D.h"

namespace PixieEngine {

class RendererOpenGL : public RendererBase {
public:
	RendererOpenGL();
	~RendererOpenGL();

	void StartFrame() override;
	void EndFrame() override;

	void DrawMesh(MeshHandle mesh) override;

	ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource) override;
	MeshHandle LoadMesh(const Mesh& mesh) override;
	void LoadMesh(MeshHandle& handle, const Mesh& mesh) override;

	TextureHandle LoadTexture(const Buffer2D<float>& texture) override;
	TextureHandle LoadTexture(const Buffer2D<glm::vec3>& texture) override;
	TextureHandle LoadTexture(const Buffer2D<glm::vec4>& texture) override;
	void LoadTexture(TextureHandle& handle, const Buffer2D<float>& texture) override;
	void LoadTexture(TextureHandle& handle, const Buffer2D<glm::vec3>& texture) override;
	void LoadTexture(TextureHandle& handle, const Buffer2D<glm::vec4>& texture) override;

	void SetTextureFiltering(const TextureHandle& handle, TextureFiltering minFilter, TextureFiltering magFilter) override;
	void SetTextureWrap(const TextureHandle& handle, TextureWrap wrapS, TextureWrap wrapT) override;
	void GenerateTextureMipmaps(const TextureHandle& handle) override;


private:
	std::vector<TextureOpenGL> m_textures;
	std::vector<MeshOpenGL> m_meshes;
	std::vector<GLuint> m_shaders;

	TextureOpenGL& GetTexture(TextureHandle handle);
	MeshOpenGL& GetMesh(MeshHandle handle);
	GLuint GetShader(ShaderHandle handle);
};

}