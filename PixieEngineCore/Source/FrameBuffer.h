#pragma once
#include "pch.h"

struct FrameBuffer {
	FrameBuffer(glm::ivec2 resolution);
	~FrameBuffer();

	void Resize(glm::ivec2 resolution);
	void Bind();
	void Unbind();

	GLuint m_frameBuffer;
	GLuint m_texture, m_depth;
	glm::ivec2 m_resolution;
};
