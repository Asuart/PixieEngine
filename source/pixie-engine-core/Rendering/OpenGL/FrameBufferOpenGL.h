#pragma once
#include "pch.h"
#include "glad/glad.h"
#include "Resources/Texture.h"
#include "Resources/MultisampleTexture.h"

namespace PixieEngine {

struct FrameBufferOpenGL {
	FrameBufferOpenGL(glm::ivec2 resolution);
	~FrameBufferOpenGL();

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

struct MultisampleFrameBufferOpenGL {
	MultisampleFrameBufferOpenGL(glm::ivec2 resolution, int32_t samples);
	~MultisampleFrameBufferOpenGL();

	void Blit(const FrameBufferOpenGL& target) const;

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

struct GBufferOpenGL {
	GBufferOpenGL(glm::ivec2 resolution);
	~GBufferOpenGL();

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

}