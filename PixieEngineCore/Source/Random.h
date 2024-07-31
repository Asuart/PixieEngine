#pragma once
#include "headers.h"
#include "RTMath.h"

float RandomFloat();
float RandomFloat(float min, float max);

glm::vec3 RandomVector();
glm::vec3 RandomVector(float min, float max);
glm::vec3 RandomUnitVector();

glm::vec3 RandomInUnitSphere();
glm::vec3 RandomInHemisphere(const glm::vec3& normal);
glm::vec3 RandomInUnitDisk();
glm::vec3 RandomCosineDirection();
