#pragma once
#include "pch.h"
#include "Ray.h"
#include "Bounds.h"
#include "RTInteraction.h"

glm::vec3 SampleUniformTriangle(glm::vec2 u);

struct ShapeSample {
	RTInteraction intr;
	float pdf;
};

class Shape {
public:
	virtual float SolidAngle(glm::vec3 p) const = 0;
	virtual bool IsValid() const = 0;
	virtual float Area() const = 0;
	virtual ShapeSample Sample(glm::vec2 u) const = 0;
	virtual Bounds3f Bounds() const = 0;
	virtual bool Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const = 0;
	virtual bool IntersectP(const Ray& ray, float tMax) const = 0;
};

class Triangle : public Shape {
public:
	glm::vec3 v0, v1, v2;
	glm::vec3 n;

	Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _n = glm::vec3(0));

	float SolidAngle(glm::vec3 p) const override;
	bool IsValid() const override;
	float Area() const override;
	ShapeSample Sample(glm::vec2 u) const override;
	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const override;
	bool IntersectP(const Ray& ray, float tMax) const override;

};

class CachedTriangle : public Shape {
public:
	glm::vec3 v0, v1, v2;
	glm::vec3 edge0, edge1, edge2;
	glm::vec3 n;
	float area;
	float d;

	CachedTriangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _n = glm::vec3(0));

	float SolidAngle(glm::vec3 p) const override;
	bool IsValid() const override;
	float Area() const override;
	ShapeSample Sample(glm::vec2 u) const override;
	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const override;
	bool IntersectP(const Ray& ray, float tMax) const override;
};

class Sphere : public Shape {
public:
	glm::vec3 c;
	float r;

	Sphere(glm::vec3 _c, float _r);

	Bounds3f Bounds() const override;
	bool Intersect(const Ray& ray, RTInteraction& out_collision, float tMax) const override;
	bool IntersectP(const Ray& ray, float tMax) const override;
};
