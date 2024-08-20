#pragma once
#include "pch.h"
#include "Ray.h"
#include "Sampler.h"

class Skybox {
public:
	virtual glm::fvec3 GetColor(const Ray& ray) = 0;

protected:
	Skybox() = default;
};

class SolidColorSkybox : public Skybox {
public:
	SolidColorSkybox(glm::fvec3 color);

	glm::fvec3 GetColor(const Ray& ray) override;
protected:
	glm::fvec3 m_color;
};

class GradientSkybox : public Skybox {
public:
	GradientSkybox(glm::fvec3 color1, glm::fvec3 color2);

	glm::fvec3 GetColor(const Ray& ray) override;

protected:
	glm::fvec3 m_color1;
	glm::fvec3 m_color2;
};