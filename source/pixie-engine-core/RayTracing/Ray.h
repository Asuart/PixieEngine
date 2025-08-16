#pragma once
#include "pch.h"

namespace PixieEngine {

class Ray {
public:
	const glm::vec3 origin;
	const glm::vec3 direction;
	const glm::vec3 inverseDirection;

	Ray(glm::vec3 origin, glm::vec3 direction) :
		origin(origin), direction(direction), inverseDirection(1.0f / direction) {
	}
};

}