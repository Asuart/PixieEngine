#include "pch.h"
#include "Texture.h"

std::map<GLuint, std::atomic<uint32_t>> Texture::s_counters;

Texture::Texture() {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::Texture(glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter) :
	m_resolution(resolution), m_internalFormat(internalFormat), m_format(format), m_type(type) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture::s_counters[m_id]++;
}

Texture::~Texture() {
	Texture::s_counters[m_id]--;
	if (Texture::s_counters[m_id] == 0) {
		glDeleteTextures(1, &m_id);
	}
}

Texture::Texture(const Texture& other) {
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	m_format = other.m_format;
	m_type = other.m_type;
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
	m_format = other.m_format;
	m_type = other.m_type;
	Texture::s_counters[m_id]++;
	return *this;
}

void Texture::SetWrap(GLint wrapS, GLint wrapT) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFilters(GLint minFilter, GLint magFilter) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetMinFilter(GLint minFilter) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetMagFilter(GLint magFilter) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}
