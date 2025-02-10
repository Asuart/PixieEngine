#include "pch.h"
#include "Texture.h"

constexpr Vec4 defaultTextureData(1.0f, 1.0f, 1.0f, 1.0f);

GLint glCastTextureWrap(TextureWrap wrap) {
	switch (wrap) {
	case TextureWrap::Reapeat: return GL_REPEAT;
	case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
	case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
	default: return GL_REPEAT;
	}
}

GLint glCastTextureFiltering(TextureFiltering filtering) {
	switch (filtering) {
	case TextureFiltering::Linear: return GL_LINEAR;
	case TextureFiltering::Nearest: return GL_NEAREST;
	case TextureFiltering::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureFiltering::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
	case TextureFiltering::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
	case TextureFiltering::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
	default: return GL_LINEAR;
	}
}

/*
	Texture
*/

Texture::Texture() {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, GL_RGBA, GL_FLOAT, &defaultTextureData[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	TextureManager::textureCounters[m_id]++;
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
	TextureManager::textureCounters[m_id]++;
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
	TextureManager::textureCounters[m_id]++;
}

Texture::Texture(glm::uvec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glCastTextureWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glCastTextureWrap(wrapT));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glCastTextureFiltering(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glCastTextureFiltering(magFilter));
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_resolution.x, m_resolution.y, 0, format, type, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	TextureManager::textureCounters[m_id]++;
}

Texture::~Texture() {
	if (!TextureManager::textureCounters.empty()) {
		TextureManager::textureCounters[m_id]--;
		if (TextureManager::textureCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
}

Texture::Texture(const Texture& other) {
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	TextureManager::textureCounters[m_id]++;
}

Texture& Texture::operator= (const Texture& other) {
	if (m_id != other.m_id) {
		TextureManager::textureCounters[m_id]--;
		if (TextureManager::textureCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	TextureManager::textureCounters[m_id]++;
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

/*
	Multi Sample texture
*/

MSTexture::MSTexture(glm::uvec2 resolution, uint32_t samples, GLint internalFormat, bool fixedSampleLocations, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat), m_samples(glm::max((uint32_t)1, samples)), m_fixedSampleLocations(fixedSampleLocations){
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_resolution.x, m_resolution.y, m_fixedSampleLocations);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	TextureManager::msTextureCounters[m_id]++;
}

MSTexture::~MSTexture() {
	if (!TextureManager::msTextureCounters.empty()) {
		TextureManager::msTextureCounters[m_id]--;
		if (TextureManager::msTextureCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
}

MSTexture::MSTexture(const MSTexture& other) {
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	m_samples = other.m_samples;
	m_fixedSampleLocations = other.m_fixedSampleLocations;
	TextureManager::msTextureCounters[m_id]++;
}

MSTexture& MSTexture::operator= (const MSTexture& other) {
	if (m_id != other.m_id) {
		TextureManager::msTextureCounters[m_id]--;
		if (TextureManager::msTextureCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	m_samples = other.m_samples;
	m_fixedSampleLocations = other.m_fixedSampleLocations;
	TextureManager::msTextureCounters[m_id]++;
	return *this;
}

void MSTexture::Resize(glm::ivec2 resolution) {
	m_resolution = glm::max(resolution, { 1, 1 });
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_resolution.x, m_resolution.y, m_fixedSampleLocations);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void MSTexture::SetSampleCount(int32_t samples) {
	m_samples = glm::max((int32_t)1, samples);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_resolution.x, m_resolution.y, m_fixedSampleLocations);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

GLuint MSTexture::GetHandle() const {
	return m_id;
}

GLint MSTexture::GetInternalFormat() const {
	return m_internalFormat;
}

glm::uvec2 MSTexture::GetResolution() const {
	return m_resolution;
}

/*
	Cubemap
*/

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
	TextureManager::cubemapCounters[m_id]++;
}

Cubemap::~Cubemap() {
	if (!TextureManager::cubemapCounters.empty()) {
		TextureManager::cubemapCounters[m_id]--;
		if (TextureManager::cubemapCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
}

Cubemap::Cubemap(const Cubemap& other) {
	m_id = other.m_id;
	TextureManager::cubemapCounters[m_id]++;
}

Cubemap& Cubemap::operator= (const Cubemap& other) {
	if (m_id != other.m_id) {
		TextureManager::cubemapCounters[m_id]--;
		if (TextureManager::cubemapCounters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	TextureManager::cubemapCounters[m_id]++;
	return *this;
}

void Cubemap::Upload(uint32_t sideIndex, glm::ivec2 resolution, GLint internalFormat, GLenum format, GLenum type, void* data) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	for (int32_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, resolution.x, resolution.y, 0, format, type, data);
	}
}

void Cubemap::GenerateMipmaps() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint Cubemap::GetHandle() const {
	return m_id;
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
