#pragma once
#include "pch.h"

struct FrameBuffer {
	FrameBuffer(glm::ivec2 resolution);
	~FrameBuffer();

	void Resize(glm::ivec2 resolution);
	void Bind();
	void Unbind();

	GLuint m_frameBuffer;
	GLuint m_texture;
	GLuint m_depth;
	glm::ivec2 m_resolution;
};

struct GBuffer {
	GBuffer(glm::ivec2 resolution);
	~GBuffer();

	void Resize(glm::ivec2 resolution);
	void Bind();
	void BindTextures();
	void Unbind();

	GLuint m_frameBuffer;
	GLuint m_albedoSpec, m_normalMetallic, m_positionRoughness;
	GLuint m_depth;
	glm::ivec2 m_resolution;
};
