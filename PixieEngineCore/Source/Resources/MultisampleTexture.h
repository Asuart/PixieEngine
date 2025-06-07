#pragma once
#include "pch.h"
#include "TextureUtils.h"

class MSTexture {
public:
	MSTexture(glm::uvec2 resolution, uint32_t samples, GLint internalFormat = GL_RGBA, bool fixedSampleLocations = false, TextureWrap wrapS = TextureWrap::Reapeat, TextureWrap wrapT = TextureWrap::Reapeat, TextureFiltering minFilter = TextureFiltering::Nearest, TextureFiltering magFilter = TextureFiltering::Nearest);
	~MSTexture();

	MSTexture(const MSTexture& other);
	MSTexture& operator= (const MSTexture& other);

	void Resize(glm::ivec2 resolution);
	void SetSampleCount(int32_t samples);

	GLuint GetHandle() const;
	GLint GetInternalFormat() const;
	glm::uvec2 GetResolution() const;

protected:
	GLuint m_id = 0;
	glm::uvec2 m_resolution = { 1, 1 };
	GLint m_internalFormat = GL_RGBA;
	int32_t m_samples = 1;
	bool m_fixedSampleLocations = false;

	static std::map<GLuint, std::atomic<uint32_t>> s_counters;
};
