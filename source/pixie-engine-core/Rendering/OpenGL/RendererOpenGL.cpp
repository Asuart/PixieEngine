#include "pch.h"
#include "RendererOpenGL.h"
#include "Debug/Log.h"
#include "OpenGLCallbacks.h"

namespace PixieEngine {

//Cubemap::Cubemap(glm::ivec2 resolution, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFiltering minFilter, TextureFiltering magFilter) {
//	glGenTextures(1, &m_id);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
//	for (int32_t i = 0; i < 6; i++) {
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glCastTextureWrap(wrapR));
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
//	Cubemap::s_counters[m_id]++;
//}
//
//void Cubemap::Upload(uint32_t sideIndex, glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data) {
//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + sideIndex, 0, internalFormat, resolution.x, resolution.y, 0, format, type, data);
//}
//
//void Cubemap::GenerateMipmaps() const {
//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
//	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//}

//void Cubemap::SetWrap(TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR) const {
//	glBindTexture(GL_TEXTURE_2D, m_id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, glCastTextureWrap(wrapR));
//	glBindTexture(GL_TEXTURE_2D, 0);
//}


//MSTexture::MSTexture(glm::uvec2 resolution, uint32_t samples, GLint internalFormat, bool fixedSampleLocations) :
//	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat), m_samples(glm::max((uint32_t)1, samples)), m_fixedSampleLocations(fixedSampleLocations) {
//	glGenTextures(1, &m_id);
//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
//	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_resolution.x, m_resolution.y, m_fixedSampleLocations);
//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
//	MSTexture::s_counters[m_id]++;
//}

void SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform1i(loc, value);
}

void SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform1ui(loc, value);
}

void SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform2i(loc, v.x, v.y);
}

void SetUniform1iv(ShaderHandle handle, const std::string& name, GLint* start, int32_t count) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform1iv(loc, count, start);
}

void SetUniform1f(ShaderHandle handle, const std::string& name, float value) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform1f(loc, value);
}

void SetUniform1fv(ShaderHandle handle, const std::string& name, GLfloat* start, int32_t count) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform1fv(loc, count, start);
}

void SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform2f(loc, v.x, v.y);
}

void SetUniform2fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform2fv(loc, count, start);
}

void SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform3f(loc, v.x, v.y, v.z);
}

void SetUniform3fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform3fv(loc, count, start);
}

void SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void SetUniform4fv(ShaderHandle handle, const std::string& name, const GLfloat* start, int32_t count) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniform4fv(loc, count, start);
}

void SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, GLboolean transpose = GL_FALSE) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniformMatrix3fv(loc, 1, transpose, &m[0][0]);
}

void SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, GLboolean transpose = GL_FALSE) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniformMatrix3fv(loc, count, transpose, start);
}

void SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, GLboolean transpose = GL_FALSE) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniformMatrix4fv(loc, 1, transpose, &m[0][0]);
}

void SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, GLboolean transpose = GL_FALSE) const {
	GLuint loc = glGetUniformLocation(m_programID, name.c_str());
	glUniformMatrix4fv(loc, count, transpose, start);
}

void SetTexture(ShaderHandle handle, const std::string& name, GLuint id, GLuint index) const {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, id);
	SetUniform1i(name, index);
}

//void SetCubemap(ShaderHandle handle, const std::string& name, GLuint id, GLuint index) const {
//	glActiveTexture(GL_TEXTURE0 + index);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
//	SetUniform1i(name, index);
//}

//inline void SetMaterial(ShaderHandle handle, const std::shared_ptr<Material> material, int32_t startTextureIndex = 0) const {
//	if (!material.get()) return;
//	SetTexture("albedoMap", material->m_albedoTexture.GetHandle(), startTextureIndex + 0);
//	SetTexture("normalMap", material->m_normalTexture.GetHandle(), startTextureIndex + 1);
//	SetTexture("metallicMap", material->m_metallicTexture.GetHandle(), startTextureIndex + 2);
//	SetTexture("roughnessMap", material->m_roughnessTexture.GetHandle(), startTextureIndex + 3);
//	SetTexture("aoMap", material->m_aoTexture.GetHandle(), startTextureIndex + 4);
//	SetUniform3f("uAlbedo", material->m_albedo);
//	SetUniform1f("uRoughness", material->m_roughness);
//	SetUniform1f("uMetallic", material->m_metallic);
//}

RendererOpenGL::RendererOpenGL() {
	if (!gladLoadGL()) {
		Log::Error("GLAD initialization failed");
		exit(2);
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OpenglCallbackHandler, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

RendererOpenGL::~RendererOpenGL() {}

void RendererOpenGL::StartFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererOpenGL::EndFrame() {

}

void RendererOpenGL::DrawMesh(MeshHandle mesh) {
	const MeshOpenGL& meshData = m_meshes[mesh.id];
	glBindVertexArray(meshData.vao);
	glDrawElements(GL_TRIANGLES, meshData.indicesCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

ShaderHandle RendererOpenGL::CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource) {

}

MeshHandle RendererOpenGL::LoadMesh(const Mesh& mesh) {
	MeshOpenGL meshData;

	glGenVertexArrays(1, &meshData.vao);
	glGenBuffers(1, &meshData.vbo);
	glGenBuffers(1, &meshData.ibo);

	glBindVertexArray(meshData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.m_vertices.size(), &mesh.m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * mesh.m_indices.size(), &mesh.m_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_meshes.push_back(meshData);

	return MeshHandle(static_cast<int32_t>(m_meshes.size() - 1));
}

TextureHandle RendererOpenGL::LoadTexture(const Buffer2D<float>& texture) {
	TextureOpenGL textureEntry;
	textureEntry.internalFormat = GL_RED;

	glGenTextures(1, &textureEntry.id);
	glBindTexture(GL_TEXTURE_2D, textureEntry.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, textureEntry.internalFormat, texture.m_resolution.x, texture.m_resolution.y, 0, GL_RED, GL_FLOAT, &texture.m_data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(textureEntry);

	return TextureHandle(static_cast<int32_t>(m_textures.size() - 1));
}

TextureHandle RendererOpenGL::LoadTexture(const Buffer2D<glm::vec3>& texture) {
	TextureOpenGL textureEntry;
	textureEntry.internalFormat = GL_RGB;

	glGenTextures(1, &textureEntry.id);
	glBindTexture(GL_TEXTURE_2D, textureEntry.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, textureEntry.internalFormat, texture.m_resolution.x, texture.m_resolution.y, 0, GL_RGB, GL_FLOAT, &texture.m_data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(textureEntry);

	return TextureHandle(static_cast<int32_t>(m_textures.size() - 1));
}

TextureHandle RendererOpenGL::LoadTexture(const Buffer2D<glm::vec4>& texture) {
	TextureOpenGL textureEntry;
	textureEntry.internalFormat = GL_RGBA;

	glGenTextures(1, &textureEntry.id);
	glBindTexture(GL_TEXTURE_2D, textureEntry.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, textureEntry.internalFormat, texture.m_resolution.x, texture.m_resolution.y, 0, GL_RGBA, GL_FLOAT, &texture.m_data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_textures.push_back(textureEntry);

	return TextureHandle(static_cast<int32_t>(m_textures.size() - 1));
}

void RendererOpenGL::SetTextureFiltering(const TextureHandle& handle, TextureFiltering minFilter, TextureFiltering magFilter) {
	const TextureOpenGL& texture = GetTexture(handle);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CastTextureFilteringOpenGL(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CastTextureFilteringOpenGL(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererOpenGL::SetTextureWrap(const TextureHandle& handle, TextureWrap wrapS, TextureWrap wrapT) {
	const TextureOpenGL& texture = GetTexture(handle);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, CastTextureWrapOpenGL(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, CastTextureWrapOpenGL(wrapT));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RendererOpenGL::GenerateTextureMipmaps(const TextureHandle& handle) {
	const TextureOpenGL& texture = GetTexture(handle);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}