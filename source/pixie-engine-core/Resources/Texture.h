#pragma once
#include "pch.h"
#include "TextureUtils.h"

namespace PixieEngine {

class Texture {
public:
	Texture();
	Texture(glm::uvec2 resolution, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	Texture(glm::uvec2 resolution, GLint internalFormat, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	Texture(glm::uvec2 resolution, GLint internalFormat, GLenum format, GLenum type, const void* data, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	~Texture();

	Texture(const Texture& other);
	Texture& operator= (const Texture& other);

	void Upload(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data);
	void Resize(glm::ivec2 resolution);
	void GenerateMipmaps() const;

	GLuint GetHandle() const;
	GLint GetInternalFormat() const;
	glm::uvec2 GetResolution() const;

	void SetWrap(TextureWrap wrapS, TextureWrap wrapT) const;
	void SetFilters(TextureFiltering minFilter, TextureFiltering magFilter) const;
	void SetMinFilter(TextureFiltering minFilter) const;
	void SetMagFilter(TextureFiltering magFilter) const;

protected:
	GLuint m_id = 0;
	glm::uvec2 m_resolution = { 1, 1 };
	GLint m_internalFormat = GL_RGBA;

	inline static std::map<GLuint, std::atomic<uint32_t>> s_counters;
};

}