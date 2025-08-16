#pragma once
#include "pch.h"

namespace PixieEngine {

struct BezierCurve2D {
	glm::vec2 p0, p1, p2, p3;

	BezierCurve2D(glm::vec2 _p0, glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _p3);

	glm::vec2 GetPoint(float t) const;
	void SetPoints(glm::vec2 _p0, glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _p3);
};

struct BezierCurve3D {
	glm::vec3 p0, p1, p2, p3;

	BezierCurve3D(glm::vec3 _p0, glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);

	glm::vec3 GetPoint(float t) const;
	void SetPoints(glm::vec3 _p0, glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);
};

}