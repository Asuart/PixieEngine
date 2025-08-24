#pragma once
#include "pch.h"
#include "ShaderHandle.h"
#include "TextureHandle.h"
#include "MeshHandle.h"
#include "TextureEnums.h"
#include "Resources/Mesh.h"

namespace PixieEngine {

class RendererBase {
public:
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void DrawMesh(MeshHandle mesh) = 0;

	virtual ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource) = 0;
	virtual MeshHandle LoadMesh(const Mesh& mesh) = 0;
	virtual void LoadMesh(MeshHandle& handle, const Mesh& mesh) = 0;

	virtual TextureHandle LoadTexture(const Buffer2D<float>& texture) = 0;
	virtual TextureHandle LoadTexture(const Buffer2D<glm::vec3>& texture) = 0;
	virtual TextureHandle LoadTexture(const Buffer2D<glm::vec4>& texture) = 0;
	virtual void LoadTexture(TextureHandle& handle, const Buffer2D<float>& texture) = 0;
	virtual void LoadTexture(TextureHandle& handle, const Buffer2D<glm::vec3>& texture) = 0;
	virtual void LoadTexture(TextureHandle& handle, const Buffer2D<glm::vec4>& texture) = 0;

	virtual void SetTextureFiltering(const TextureHandle& handle, TextureFiltering minFilter, TextureFiltering magFilter) = 0;
	virtual void SetTextureWrap(const TextureHandle& handle, TextureWrap wrapS, TextureWrap wrapT) = 0;
	virtual void GenerateTextureMipmaps(const TextureHandle& handle) = 0;

	virtual void SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value) const;
	virtual void SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value) const;
	virtual void SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v) const;
	virtual void SetUniform1iv(ShaderHandle handle, const std::string& name, int32_t* data, int32_t count) const;
	virtual void SetUniform1f(ShaderHandle handle, const std::string& name, float value) const;
	virtual void SetUniform1fv(ShaderHandle handle, const std::string& name, float* data, int32_t count) const;
	virtual void SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v) const;
	virtual void SetUniform2fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const;
	virtual void SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v) const;
	virtual void SetUniform3fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const;
	virtual void SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v) const;
	virtual void SetUniform4fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const;
	virtual void SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, GLboolean transpose = GL_FALSE) const;
	virtual void SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, GLboolean transpose = GL_FALSE) const;
	virtual void SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, GLboolean transpose = GL_FALSE) const;
	virtual void SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, GLboolean transpose = GL_FALSE) const;
	virtual void BindTexture(ShaderHandle handle, const std::string& name, GLuint id, GLuint index) const;
};

}