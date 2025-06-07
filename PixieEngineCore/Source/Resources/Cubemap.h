#pragma once
#include "pch.h"
#include "TextureUtils.h"

class Cubemap {
public:
	Cubemap(glm::ivec2 resolution, TextureWrap wrapS = TextureWrap::ClampToEdge, TextureWrap wrapT = TextureWrap::ClampToEdge, TextureWrap wrapR = TextureWrap::ClampToEdge, TextureFiltering minFilter = TextureFiltering::Linear, TextureFiltering magFilter = TextureFiltering::Linear);
	~Cubemap();

	Cubemap(const Cubemap& other);
	Cubemap& operator= (const Cubemap& other);

	void Upload(uint32_t sideIndex, glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data);
	void GenerateMipmaps() const;

	GLuint GetHandle() const;
	glm::ivec2 GetResolution() const;

	void SetWrap(TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR) const;
	void SetFilters(TextureFiltering minFilter, TextureFiltering magFilter) const;
	void SetMinFilter(TextureFiltering minFilter) const;
	void SetMagFilter(TextureFiltering magFilter) const;

protected:
	GLuint m_id = 0;
	glm::ivec2 m_resolution = { 0, 0 };

	static std::map<GLuint, std::atomic<uint32_t>> s_counters;
};
