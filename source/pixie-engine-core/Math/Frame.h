#pragma once
#include "pch.h"
#include "MathBase.h"

namespace PixieEngine {

struct Frame {
	glm::vec3 x, y, z;

	Frame();
	Frame(glm::vec3 x, glm::vec3 y, glm::vec3 z);
	static Frame FromXZ(glm::vec3 x, glm::vec3 z);
	static Frame FromXY(glm::vec3 x, glm::vec3 y);
	static Frame FromZ(glm::vec3 z);
	static Frame FromX(glm::vec3 x);
	static Frame FromY(glm::vec3 y);

	glm::vec3 ToLocal(glm::vec3 v) const;
	glm::vec3 FromLocal(glm::vec3 v) const;
};

}