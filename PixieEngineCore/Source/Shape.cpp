#include "Shape.h"

glm::vec3 SampleUniformTriangle(glm::vec2 u) {
	float b0, b1;
	if (u[0] < u[1]) {
		b0 = u[0] / 2;
		b1 = u[1] - b0;
	}
	else {
		b1 = u[1] / 2;
		b0 = u[0] - b1;
	}
	return { b0, b1, 1 - b0 - b1 };
}

Triangle::Triangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _n)
	: v0(_v0), v1(_v1), v2(_v2) {
	if (_n == glm::vec3(0)) {
		n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}
	else {
		n = glm::normalize(_n);
	}
}

float Triangle::SolidAngle(glm::vec3 p) const {
	return SphericalTriangleArea(glm::normalize(v0 - p), glm::normalize(v1 - p), glm::normalize(v2 - p));
}

bool Triangle::IsValid() const {
	if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) return false;
	return true;
}

float Triangle::Area() const {
	return glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5;
}

ShapeSample Triangle::Sample(glm::vec2 u) const {
	RTInteraction intr;
	glm::vec3 b = SampleUniformTriangle(u);
	intr.p = b[0] * v0 + b[1] * v1 + b[2] * v2;
	intr.n = n;
	return ShapeSample(intr, 1.0 / Area());
}

Bounds3f Triangle::Bounds() const {
	return Union(Bounds3f(v0, v1), Bounds3f(v2));
}

bool Triangle::Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const {
	float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	float d = glm::dot(n, v0);
	float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	glm::vec3 P = ray.At(t);
	glm::vec3 C;

	glm::vec3 edge0 = v1 - v0;
	glm::vec3 edge1 = v2 - v1;
	glm::vec3 edge2 = v0 - v2;

	glm::vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	float area = glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5;

	outCollision.distance = t;
	outCollision.backFace = NdotRayDirection < 0;
	outCollision.n = outCollision.backFace ? n : -n;
	outCollision.p = P;
	outCollision.uv = glm::vec3(0);
	outCollision.area = area;

	return true;
}

bool Triangle::IntersectP(const Ray& ray, float tMax) const {
	float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	float d = glm::dot(n, v0);
	float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	glm::vec3 P = ray.At(t);
	glm::vec3 C;

	glm::vec3 edge0 = v1 - v0;
	glm::vec3 edge1 = v2 - v1;
	glm::vec3 edge2 = v0 - v2;

	glm::vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	return true;
}

CachedTriangle::CachedTriangle(glm::vec3 _v0, glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _n)
	: v0(_v0), v1(_v1), v2(_v2) {
	edge0 = v1 - v0;
	edge1 = v2 - v1;
	edge2 = v0 - v2;
	if (_n == glm::vec3(0)) {
		n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}
	else {
		n = glm::normalize(_n);
	}
	area = glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5;
	d = glm::dot(n, v0);
}

float CachedTriangle::SolidAngle(glm::vec3 p) const {
	return SphericalTriangleArea(glm::normalize(v0 - p), glm::normalize(v1 - p), glm::normalize(v2 - p));
}

bool CachedTriangle::IsValid() const {
	if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) return false;
	return true;
}

float CachedTriangle::Area() const {
	return area;
}

ShapeSample CachedTriangle::Sample(glm::vec2 u) const {
	RTInteraction intr;
	glm::vec3 b = SampleUniformTriangle(u);
	intr.p = b[0] * v0 + b[1] * v1 + b[2] * v2;
	intr.n = n;
	return ShapeSample(intr, 1.0 / Area());
}

Bounds3f CachedTriangle::Bounds() const {
	return Union(Bounds3f(v0, v1), Bounds3f(v2));
}

bool CachedTriangle::Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const {
	float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	glm::vec3 P = ray.At(t);
	glm::vec3 C;

	glm::vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	outCollision.distance = t;
	outCollision.backFace = NdotRayDirection < 0;
	outCollision.n = outCollision.backFace ? n : -n;
	outCollision.p = P;
	outCollision.uv = glm::vec3(0);
	outCollision.area = area;
	outCollision.dpdus = edge0;

	return true;
}

bool CachedTriangle::IntersectP(const Ray& ray, float tMax) const {
	float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	glm::vec3 P = ray.At(t);
	glm::vec3 C;

	glm::vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	glm::vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	return true;
}

Sphere::Sphere(glm::vec3 _c, float _r)
	: c(_c), r(_r) {}

Bounds3f Sphere::Bounds() const {
	return Bounds3f(glm::vec3(-r), glm::vec3(r));
}

bool Sphere::Intersect(const Ray& ray, RTInteraction& out_collision, float tMax) const {
	glm::vec3 oc = ray.o - c;
	float a = glm::length2(ray.d);
	float half_b = glm::dot(oc, ray.d);
	float c = glm::length2(oc) - r * r;

	float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	float sqrtd = sqrt(discriminant);

	float root = (-half_b - sqrtd) / a;
	if (root < ShadowEpsilon || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < ShadowEpsilon || tMax < root)
			return false;
	}

	out_collision.distance = root;
	out_collision.p = ray.At(root);
	glm::vec3 outNormal = (out_collision.p - c) / r;
	float theta = acos(-outNormal.y);
	float phi = atan2(-outNormal.z, outNormal.x) + Pi;
	out_collision.uv = glm::vec2(phi / (2.0f * Pi), theta / Pi);
	bool front_face = glm::dot(ray.d, outNormal) < 0;
	out_collision.n = front_face ? outNormal : -outNormal;
	out_collision.backFace = !front_face;

	return true;
}

bool Sphere::IntersectP(const Ray& ray, float tMax) const {
	glm::vec3 oc = ray.o - c;
	float a = glm::length2(ray.d);
	float half_b = glm::dot(oc, ray.d);
	float c = glm::length2(oc) - r * r;

	float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	float sqrtd = sqrt(discriminant);

	float root = (-half_b - sqrtd) / a;
	if (root < ShadowEpsilon || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < ShadowEpsilon || tMax < root)
			return false;
	}

	return true;
}