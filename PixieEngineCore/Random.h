#ifndef PIXIE_ENGINE_RANDOM
#define PIXIE_ENGINE_RANDOM

#include "pch.h"
#include "Math.h"

float RandomFloat();
float RandomFloat(float min, float max);

glm::vec3 RandomVector();
glm::vec3 RandomVector(float min, float max);
glm::vec3 RandomUnitVector();

glm::vec3 RandomInUnitSphere();
glm::vec3 RandomInHemisphere(const glm::vec3& normal);
glm::vec3 RandomInUnitDisk();
glm::vec3 RandomCosineDirection();

#endif // PIXIE_ENGINE_RANDOM