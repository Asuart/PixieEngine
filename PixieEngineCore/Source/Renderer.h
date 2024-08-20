#pragma once
#include "pch.h"

class Renderer {
public:
	glm::ivec2 GetResolution();
	virtual void SetResolution(const glm::ivec2& resolution);
	virtual void Reset() = 0;

protected:
	Renderer(const glm::ivec2 resolution);

	glm::ivec2 m_resolution;
};