#include "pch.h"
#include "Cubemap.h"

std::map<GLuint, std::atomic<uint32_t>> Cubemap::s_counters;

Cubemap::Cubemap(glm::ivec2 resolution, TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR, TextureFiltering minFilter, TextureFiltering magFilter) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	for (int32_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, glCastTextureWrap(wrapR));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	Cubemap::s_counters[m_id]++;
}

Cubemap::~Cubemap() {
	if (!Cubemap::s_counters.empty()) {
		Cubemap::s_counters[m_id]--;
		if (Cubemap::s_counters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
}

Cubemap::Cubemap(const Cubemap& other) {
	m_id = other.m_id;
	Cubemap::s_counters[m_id]++;
}

Cubemap& Cubemap::operator= (const Cubemap& other) {
	if (m_id != other.m_id) {
		Cubemap::s_counters[m_id]--;
		if (Cubemap::s_counters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	Cubemap::s_counters[m_id]++;
	return *this;
}

void Cubemap::Upload(uint32_t sideIndex, glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + sideIndex, 0, internalFormat, resolution.x, resolution.y, 0, format, type, data);
}

void Cubemap::GenerateMipmaps() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint Cubemap::GetHandle() const {
	return m_id;
}

glm::ivec2 Cubemap::GetResolution() const {
	return m_resolution;
}

void Cubemap::SetWrap(TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, glCastTextureWrap(wrapR));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Cubemap::SetFilters(TextureFiltering minFilter, TextureFiltering magFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Cubemap::SetMinFilter(TextureFiltering minFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Cubemap::SetMagFilter(TextureFiltering magFilter) const {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}
