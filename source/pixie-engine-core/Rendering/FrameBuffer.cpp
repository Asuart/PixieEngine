#include "pch.h"
#include "FrameBuffer.h"

namespace PixieEngine {

/*
	FrameBuffer
*/

FrameBuffer::FrameBuffer(glm::ivec2 resolution) :
	m_resolution(resolution) {
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw "Failed to initializa FrameBuffer";
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::ivec2 FrameBuffer::GetResolution() const {
	return m_resolution;
}

GLuint FrameBuffer::GetBufferHandle() const {
	return m_frameBuffer;
}

GLuint FrameBuffer::GetColorHandle() const {
	return m_texture;
}

GLuint FrameBuffer::GetDepthHandle() const {
	return m_depth;
}

void FrameBuffer::ResizeViewport() const {
	glViewport(0, 0, m_resolution.x, m_resolution.y);
}

void FrameBuffer::Clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

void FrameBuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
	MultisampleFrameBuffer
*/

MultisampleFrameBuffer::MultisampleFrameBuffer(glm::ivec2 resolution, int32_t samples) :
	m_resolution(resolution), m_samples(samples), m_texture(resolution, samples, GL_RGBA) {
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texture.GetHandle());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_texture.GetHandle(), 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	//glGenTextures(1, &m_depth);
	//glBindTexture(GL_TEXTURE_2D, m_depth);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw "Failed to initializa FrameBuffer";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MultisampleFrameBuffer::~MultisampleFrameBuffer() {
	glDeleteFramebuffers(1, &m_frameBuffer);
	glDeleteTextures(1, &m_depth);
}

void MultisampleFrameBuffer::Blit(const FrameBuffer& target) const {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetBufferHandle());
	glBlitFramebuffer(0, 0, m_resolution.x, m_resolution.y, 0, 0, target.GetResolution().x, target.GetResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void MultisampleFrameBuffer::Resize(glm::ivec2 resolution) {
	if (m_resolution == resolution) {
		return;
	}
	m_resolution = resolution;
	m_texture.Resize(resolution);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::ivec2 MultisampleFrameBuffer::GetResolution() const {
	return m_resolution;
}

void MultisampleFrameBuffer::SetSampleCount(int32_t samples) {
	m_samples = samples;
	m_texture.SetSampleCount(samples);
}

int32_t MultisampleFrameBuffer::GetSampleCount() const {
	return m_samples;
}

GLuint MultisampleFrameBuffer::GetBufferHandle() const {
	return m_frameBuffer;
}

GLuint MultisampleFrameBuffer::GetColorHandle() const {
	return m_texture.GetHandle();
}

GLuint MultisampleFrameBuffer::GetDepthHandle() const {
	return m_depth;
}

void MultisampleFrameBuffer::ResizeViewport() const {
	glViewport(0, 0, m_resolution.x, m_resolution.y);
}

void MultisampleFrameBuffer::Clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MultisampleFrameBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

void MultisampleFrameBuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
	GBuffer
*/

GBuffer::GBuffer(glm::ivec2 resolution) :
	m_resolution(resolution) {
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_albedoSpec);
	glBindTexture(GL_TEXTURE_2D, m_albedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedoSpec, 0);

	glGenTextures(1, &m_positionRoughness);
	glBindTexture(GL_TEXTURE_2D, m_positionRoughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_positionRoughness, 0);

	glGenTextures(1, &m_normalMetallic);
	glBindTexture(GL_TEXTURE_2D, m_normalMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_normalMetallic, 0);

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw "Failed to initializa FrameBuffer";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GBuffer::~GBuffer() {
	glDeleteFramebuffers(1, &m_frameBuffer);
	glDeleteTextures(1, &m_albedoSpec);
	glDeleteTextures(1, &m_positionRoughness);
	glDeleteTextures(1, &m_normalMetallic);
	glDeleteTextures(1, &m_depth);
}

void GBuffer::Resize(glm::ivec2 resolution) {
	if (m_resolution == resolution) {
		return;
	}
	m_resolution = resolution;
	glBindTexture(GL_TEXTURE_2D, m_albedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, m_positionRoughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, m_normalMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, m_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, resolution.x, resolution.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GBuffer::ResizeViewport() const {
	glViewport(0, 0, m_resolution.x, m_resolution.y);
}

void GBuffer::Clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	BindTextures();
}

void GBuffer::BindTextures() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoSpec);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_positionRoughness);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_normalMetallic);
}

void GBuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}