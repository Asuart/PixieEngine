#include "Random.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);

inline float RandomFloat() {
	return dist(gen);
}

inline float RandomFloat(float min, float max) {
	return min + (max - min) * RandomFloat();
}

inline glm::vec3 RandomVector() {
	return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline glm::vec3 RandomVector(float min, float max) {
	return glm::vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

inline glm::vec3 RandomUnitVector() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

glm::vec3 RandomInUnitSphere() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

glm::vec3 RandomInHemisphere(const glm::vec3& normal) {
	glm::vec3 uSphere = RandomUnitVector();
	if (dot(uSphere, normal) > 0.0) // In the same hemisphere as the normal
		return uSphere;
	return -uSphere;
}

inline glm::vec3 RandomInUnitDisk() {
	RandomInUnitSphere();
	return glm::normalize(glm::vec3(RandomFloat(-1, 1), RandomFloat(-1, 1), 0));
}

glm::vec3 RandomCosineDirection() {
	float r1 = RandomFloat();
	float r2 = RandomFloat();
	float z = sqrt(1 - r2);

	float phi = 2 * Pi * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);

	return glm::vec3(x, y, z);
}