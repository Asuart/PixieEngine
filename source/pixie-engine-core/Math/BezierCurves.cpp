#include "pch.h"
#include "BezierCurves.h"

namespace PixieEngine {

BezierCurve2D::BezierCurve2D(glm::vec2 _p0, glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _p3) :
	p0(_p0), p1(_p1), p2(_p2), p3(_p3) {
}

glm::vec2 BezierCurve2D::GetPoint(float t) const {
	float oneMinusT = 1.0f - t;
	float oneMinusT2 = oneMinusT * oneMinusT;
	float oneMinusT3 = oneMinusT2 * oneMinusT;
	float t2 = t * t;
	float t3 = t2 * t;
	return oneMinusT3 * p0 + 3 * oneMinusT2 * t * p1 + 3 * oneMinusT * t2 * p2 + t3 * p3;
}

void BezierCurve2D::SetPoints(glm::vec2 _p0, glm::vec2 _p1, glm::vec2 _p2, glm::vec2 _p3) {
	p0 = _p0;
	p1 = _p1;
	p2 = _p2;
	p3 = _p3;
}



BezierCurve3D::BezierCurve3D(glm::vec3 _p0, glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3) :
	p0(_p0), p1(_p1), p2(_p2), p3(_p3) {
}

glm::vec3 BezierCurve3D::GetPoint(float t) const {
	float oneMinusT = 1.0f - t;
	float oneMinusT2 = oneMinusT * oneMinusT;
	float oneMinusT3 = oneMinusT2 * oneMinusT;
	float t2 = t * t;
	float t3 = t2 * t;
	return oneMinusT3 * p0 + 3 * oneMinusT2 * t * p1 + 3 * oneMinusT * t2 * p2 + t3 * p3;
}

void BezierCurve3D::SetPoints(glm::vec3 _p0, glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3) {
	p0 = _p0;
	p1 = _p1;
	p2 = _p2;
	p3 = _p3;
}

}