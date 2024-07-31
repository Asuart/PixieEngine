#pragma once
#include "RTInteraction.h"

class VisibleSurface {
public:
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
	glm::vec3 albedo;
	bool set = false;

	VisibleSurface(const RTInteraction& si, glm::vec3 albedo);

	operator bool() const;
};
