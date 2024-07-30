#ifndef PIXIE_ENGINE_FRAME_BUFFER
#define PIXIE_ENGINE_FRAME_BUFFER

#include "pch.h"

struct FrameBuffer {
	FrameBuffer();
	~FrameBuffer();

	void Resize(uint32_t width, uint32_t height);
	void Bind();
	void Unbind();

	GLuint m_frameBuffer;
	GLuint m_texture, m_depth;
	glm::ivec2 m_resolution = {1280, 720};
};

#endif // PIXIE_ENGINE_FRAME_BUFFER