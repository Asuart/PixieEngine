#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTInteraction.h"
#include "Shape.h"

struct LightLiSample {
	glm::vec3 L = glm::vec3(0);
	glm::vec3 wi = glm::vec3(0);
	float pdf = 0;
	RTInteraction pLight;

	LightLiSample(const glm::vec3& L, glm::vec3 wi, float pdf, const RTInteraction& pLight);
};

struct LightSampleContext {
	glm::vec3 p;
	glm::vec3 n;
};

class DiffuseAreaLight {
public:
	Shape* shape = nullptr;
	bool twoSided = true;
	glm::vec3 Lemit = glm::vec3(1.0);
	float scale = 1.0;

	DiffuseAreaLight(Shape* shape, glm::vec3 emit = glm::vec3(1), float scale = 1);

	glm::vec3 L(glm::vec3 p, glm::vec3 n, glm::vec2 uv, glm::vec3 w) const;
	std::optional<LightLiSample> SampleLi(RTInteraction intr, glm::vec2 u) const;
};

struct SampledLight {
	DiffuseAreaLight light;
	float p = 0;
};

class UniformLightSampler {
	std::vector<DiffuseAreaLight> lights;
public:

	UniformLightSampler(const std::vector<DiffuseAreaLight>& _lights);

	std::optional<SampledLight> Sample(float u) const;
	float PMF() const;
};
