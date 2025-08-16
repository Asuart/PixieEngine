#pragma once
#include "pch.h"
#include "Math/MathBase.h"
#include "Math/Transform.h"

namespace PixieEngine {

struct DirectionCone {
	glm::vec3 w = glm::vec3(0);
	float cosTheta = Infinity;

	DirectionCone() = default;
	DirectionCone(glm::vec3 w, float cosTheta);
	static DirectionCone EntireSphere();

	bool IsEmpty() const;
};

bool Inside(const DirectionCone& d, glm::vec3 w);

}