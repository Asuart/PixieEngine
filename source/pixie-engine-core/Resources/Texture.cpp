#include "pch.h"
#include "Texture.h"

constexpr glm::vec4 defaultTextureData(1.0f, 1.0f, 1.0f, 1.0f);

std::map<GLuint, std::atomic<uint32_t>> Texture::s_counters;

Texture::Texture() {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, GL_RGBA, GL_FLOAT, &defaultTextureData[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::Texture(glm::uvec2 resolution, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::Texture(glm::uvec2 resolution, GLint internalFormat, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::Texture(glm::uvec2 resolution, GLint internalFormat, GLenum format, GLenum type, const void* data, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, format, type, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::~Texture() {
	if (!Texture::s_counters.empty()) {
		Texture::s_counters[m_id]--;
		if (Texture::s_counters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
}

Texture::Texture(const Texture& other) {
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	Texture::s_counters[m_id]++;
}

Texture& Texture::operator= (const Texture& other) {
	if (m_id != other.m_id) {
		Texture::s_counters[m_id]--;
		if (Texture::s_counters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	Texture::s_counters[m_id]++;
	return *this;
}

void Texture::Upload(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data) {
	m_resolution = glm::max(resolution, { 1, 1 });
	m_internalFormat = internalFormat;
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_resolution.x, m_resolution.y, 0, format, type, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Resize(glm::ivec2 resolution) {
	m_resolution = glm::max(resolution, { 1, 1 });
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GenerateMipmaps() const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::GetHandle() const {
	return m_id;
}

GLint Texture::GetInternalFormat() const {
	return m_internalFormat;
}

glm::uvec2 Texture::GetResolution() const {
	return m_resolution;
}

void Texture::SetWrap(TextureWrap wrapS, TextureWrap wrapT) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilters(TextureFiltering minFilter, TextureFiltering magFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetMinFilter(TextureFiltering minFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetMagFilter(TextureFiltering magFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}
