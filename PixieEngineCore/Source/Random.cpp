#include "pch.h"
#include "Random.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);

inline Float RandomFloat() {
	return (Float)dist(gen);
}

inline Float RandomFloat(Float min, Float max) {
	return min + (max - min) * RandomFloat();
}

inline Vec3 RandomVector() {
	return Vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline Vec3 RandomVector(Float min, Float max) {
	return Vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

inline Vec3 RandomUnitVector() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

Vec3 RandomInUnitSphere() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

Vec3 RandomInHemisphere(const Vec3& normal) {
	Vec3 uSphere = RandomUnitVector();
	if (dot(uSphere, normal) > 0.0) // In the same hemisphere as the normal
		return uSphere;
	return -uSphere;
}

inline Vec3 RandomInUnitDisk() {
	RandomInUnitSphere();
	return glm::normalize(Vec3(RandomFloat(-1, 1), RandomFloat(-1, 1), 0));
}

Vec3 RandomCosineDirection() {
	Float r1 = RandomFloat();
	Float r2 = RandomFloat();
	Float z = sqrt(1 - r2);

	Float phi = 2 * Pi * r1;
	Float x = cos(phi) * sqrt(r2);
	Float y = sin(phi) * sqrt(r2);

	return Vec3(x, y, z);
}