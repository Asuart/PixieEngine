#ifndef PIXIE_ENGINE_CORE_RAY
#define PIXIE_ENGINE_CORE_RAY

#include "pch.h"
#include "Math.h"

struct Ray {
	glm::vec3 o;
	glm::vec3 d;

	Ray(const glm::vec3& _o = glm::vec3(0.0), const glm::vec3& _d = glm::vec3(0.0));

	glm::vec3 At(float t) const;
};

#endif // PIXIE_ENGINE_CORE_RAY