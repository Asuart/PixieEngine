#pragma once
#include "pch.h"

struct Texture {
	GLuint m_id = 0;
	glm::ivec2 m_resolution = { 1, 1 };
	GLint m_internalFormat = GL_RGBA;
	GLenum m_format = GL_RGBA;
	GLenum m_type = GL_FLOAT;

	Texture();
	Texture(glm::ivec2 resolution, GLuint textureID);
	Texture(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT, GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST);
	Texture(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT, GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST);
	~Texture();

	Texture(const Texture& other);
	Texture& operator= (const Texture& other);

	void SetWrap(GLint wrapS, GLint wrapT);
	void SetFilters(GLint minFilter, GLint magFilter);
	void SetMinFilter(GLint minFilter);
	void SetMagFilter(GLint magFilter);

protected:
	static std::map<GLuint, std::atomic<uint32_t>> s_counters;

public:
	static size_t GetActiveTexturesCount();

	friend class ResourceManager;
};

struct CubemapTexture {
	// TODO
};

struct MSTexture {
	// TODO
};
