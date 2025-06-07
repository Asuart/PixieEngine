#include "pch.h"
#include "MultisampleTexture.h"

std::map<GLuint, std::atomic<uint32_t>> MSTexture::s_counters;

MSTexture::MSTexture(glm::uvec2 resolution, uint32_t samples, GLint internalFormat, bool fixedSampleLocations, TextureWrap wrapS, TextureWrap wrapT, TextureFiltering minFilter, TextureFiltering magFilter) :
	m_resolution(glm::max(resolution, { 1, 1 })), m_internalFormat(internalFormat), m_samples(glm::max((uint32_t)1, samples)), m_fixedSampleLocations(fixedSampleLocations) {
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_resolution.x, m_resolution.y, m_fixedSampleLocations);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	MSTexture::s_counters[m_id]++;
}

MSTexture::~MSTexture() {
	if (!MSTexture::s_counters.empty()) {
		MSTexture::s_counters[m_id]--;
		if (MSTexture::s_counters[m_id] == 0) {
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
	MSTexture::s_counters[m_id]++;
}

MSTexture& MSTexture::operator= (const MSTexture& other) {
	if (m_id != other.m_id) {
		MSTexture::s_counters[m_id]--;
		if (MSTexture::s_counters[m_id] == 0) {
			glDeleteTextures(1, &m_id);
		}
	}
	m_id = other.m_id;
	m_resolution = other.m_resolution;
	m_internalFormat = other.m_internalFormat;
	m_samples = other.m_samples;
	m_fixedSampleLocations = other.m_fixedSampleLocations;
	MSTexture::s_counters[m_id]++;
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
