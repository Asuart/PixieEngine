#pragma once
#include "pch.h"
#include "Resources/Texture.h"
#include "Resources/MultisampleTexture.h"

struct FrameBuffer {
	FrameBuffer(glm::ivec2 resolution);
	~FrameBuffer();

	void Resize(glm::ivec2 resolution);
	glm::ivec2 GetResolution() const;
	GLuint GetBufferHandle() const;
	GLuint GetColorHandle() const;
	GLuint GetDepthHandle() const;
	void ResizeViewport() const;
	void Clear() const;
	void Bind() const;
	void Unbind() const;

protected:
	GLuint m_frameBuffer;
	GLuint m_texture;
	GLuint m_depth;
	glm::ivec2 m_resolution;
};

struct MultisampleFrameBuffer {
	MultisampleFrameBuffer(glm::ivec2 resolution, int32_t samples);
	~MultisampleFrameBuffer();

	void Blit(const FrameBuffer& target) const;

	void Resize(glm::ivec2 resolution);
	glm::ivec2 GetResolution() const;
	void SetSampleCount(int32_t samples);
	int32_t GetSampleCount() const;
	GLuint GetBufferHandle() const;
	GLuint GetColorHandle() const;
	GLuint GetDepthHandle() const;
	void ResizeViewport() const;
	void Clear() const;
	void Bind() const;
	void Unbind() const;

protected:
	glm::ivec2 m_resolution;
	GLuint m_frameBuffer;
	MSTexture m_texture;
	GLuint m_depth;
	int32_t m_samples;
};

struct GBuffer {
	GBuffer(glm::ivec2 resolution);
	~GBuffer();

	void Resize(glm::ivec2 resolution);
	void ResizeViewport() const;
	void Clear() const;
	void Bind() const;
	void BindTextures() const;
	void Unbind() const;

	GLuint m_frameBuffer;
	GLuint m_albedoSpec, m_normalMetallic, m_positionRoughness;
	GLuint m_depth;
	glm::ivec2 m_resolution;
};

template<GLuint internalFormat, GLuint format, GLuint type>
struct PostProcessingFrameBuffer {
	PostProcessingFrameBuffer(glm::ivec2 resolution) :
		m_resolution(resolution) {
		glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resolution.x, resolution.y, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw "Failed to initializa FrameBuffer";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	~PostProcessingFrameBuffer() {
		glDeleteFramebuffers(1, &m_frameBuffer);
		glDeleteTextures(1, &m_texture);
	}

	void Resize(glm::ivec2 resolution) {
		if (m_resolution == resolution) {
			return;
		}
		m_resolution = resolution;
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void ResizeViewport() const {
		glViewport(0, 0, m_resolution.x, m_resolution.y);
	}

	void Clear() const {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	}

	void Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint m_frameBuffer;
	GLuint m_texture;
	glm::ivec2 m_resolution;
};
