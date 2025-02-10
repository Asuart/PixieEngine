#pragma once
#include "pch.h"
#include "TextureManager.h"

enum class TextureWrap {
	Reapeat = 0,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
};

enum class TextureFiltering {
	Nearest,
	Linear,
	NearestMipmapNearest,
	LinearMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapLinear,
};

GLint glCastTextureWrap(TextureWrap wrap);
GLint glCastTextureFiltering(TextureFiltering filtering);

class Texture {
public:
	Texture();
	Texture(glm::uvec2 resolution, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	Texture(glm::uvec2 resolution, GLint internalFormat, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	Texture(glm::uvec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
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
};

class MSTexture {
public:
	MSTexture(glm::uvec2 resolution, uint32_t samples, GLint internalFormat = GL_RGBA, bool fixedSampleLocations = false, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	~MSTexture();

	MSTexture(const MSTexture& other);
	MSTexture& operator= (const MSTexture& other);

	void Resize(glm::ivec2 resolution);
	void SetSampleCount(int32_t samples);

	GLuint GetHandle() const;
	GLint GetInternalFormat() const;
	glm::uvec2 GetResolution() const;

protected:
	GLuint m_id = 0;
	glm::uvec2 m_resolution = { 1, 1 };
	GLint m_internalFormat = GL_RGBA;
	int32_t m_samples = 1;
	bool m_fixedSampleLocations = false;
};

class Cubemap {
public:
	Cubemap(glm::ivec2 resolution, TextureWrap wrapS = TextureWrap::ClampToEdge, TextureWrap wrapT = TextureWrap::ClampToEdge, TextureWrap wrapR = TextureWrap::ClampToEdge, TextureFiltering minFilter = TextureFiltering::Linear, TextureFiltering magFilter = TextureFiltering::Linear);
	~Cubemap();

	Cubemap(const Cubemap& other);
	Cubemap& operator= (const Cubemap& other);

	void Upload(uint32_t sideIndex, glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data);
	void GenerateMipmaps() const;

	GLuint GetHandle() const;

	void SetWrap(TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR) const;
	void SetFilters(TextureFiltering minFilter, TextureFiltering magFilter) const;
	void SetMinFilter(TextureFiltering minFilter) const;
	void SetMagFilter(TextureFiltering magFilter) const;

protected:
	GLuint m_id = 0;
};
