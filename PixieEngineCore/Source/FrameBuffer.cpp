#include "pch.h"
#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(glm::ivec2 resolution) :
	m_resolution(resolution) {
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw "Failed to initializa FrameBuffer\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &m_frameBuffer);
	glDeleteTextures(1, &m_texture);
	glDeleteTextures(1, &m_depth);
}

void FrameBuffer::Resize(glm::ivec2 resolution) {
	if (m_resolution == resolution) {
		return;
	}
	m_resolution = resolution;
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
