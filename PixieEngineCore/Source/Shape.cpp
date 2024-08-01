
#include "Shape.h"

Vec3 SampleUniformTriangle(Vec2 u) {
	Float b0, b1;
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

Triangle::Triangle(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _n)
	: v0(_v0), v1(_v1), v2(_v2) {
	if (_n == Vec3(0)) {
		n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}
	else {
		n = glm::normalize(_n);
	}
}

Float Triangle::SolidAngle(Vec3 p) const {
	return SphericalTriangleArea(glm::normalize(v0 - p), glm::normalize(v1 - p), glm::normalize(v2 - p));
}

bool Triangle::IsValid() const {
	if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) return false;
	return true;
}

Float Triangle::Area() const {
	return glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5f;
}

ShapeSample Triangle::Sample(Vec2 u) const {
	RTInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.p = b[0] * v0 + b[1] * v1 + b[2] * v2;
	intr.n = n;
	return ShapeSample(intr, 1.0f / Area());
}

Bounds3f Triangle::Bounds() const {
	return Union(Bounds3f(v0, v1), Bounds3f(v2));
}

bool Triangle::Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax) const {
	Float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	Float d = glm::dot(n, v0);
	Float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 edge0 = v1 - v0;
	Vec3 edge1 = v2 - v1;
	Vec3 edge2 = v0 - v2;

	Vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	Float area = glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5f;

	outCollision.distance = t;
	outCollision.backFace = NdotRayDirection < 0;
	outCollision.n = outCollision.backFace ? n : -n;
	outCollision.p = P;
	outCollision.uv = Vec3(0);
	outCollision.area = area;

	return true;
}

bool Triangle::IntersectP(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	Float d = glm::dot(n, v0);
	Float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 edge0 = v1 - v0;
	Vec3 edge1 = v2 - v1;
	Vec3 edge2 = v0 - v2;

	Vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	return true;
}

CachedTriangle::CachedTriangle(Vec3 _v0, Vec3 _v1, Vec3 _v2, Vec3 _n)
	: v0(_v0), v1(_v1), v2(_v2) {
	edge0 = v1 - v0;
	edge1 = v2 - v1;
	edge2 = v0 - v2;
	if (_n == Vec3(0.0f)) {
		n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}
	else {
		n = glm::normalize(_n);
	}
	area = glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5f;
	d = glm::dot(n, v0);
}

Float CachedTriangle::SolidAngle(Vec3 p) const {
	return SphericalTriangleArea(glm::normalize(v0 - p), glm::normalize(v1 - p), glm::normalize(v2 - p));
}

bool CachedTriangle::IsValid() const {
	if (std::isnan(n.x) || std::isnan(n.y) || std::isnan(n.z)) return false;
	return true;
}

Float CachedTriangle::Area() const {
	return area;
}

ShapeSample CachedTriangle::Sample(Vec2 u) const {
	RTInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.p = b[0] * v0 + b[1] * v1 + b[2] * v2;
	intr.n = n;
	return ShapeSample(intr, 1.0f / Area());
}

Bounds3f CachedTriangle::Bounds() const {
	return Union(Bounds3f(v0, v1), Bounds3f(v2));
}

bool CachedTriangle::Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax) const {
	Float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	Float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	outCollision.distance = t;
	outCollision.backFace = NdotRayDirection < 0;
	outCollision.n = outCollision.backFace ? n : -n;
	outCollision.p = P;
	outCollision.uv = Vec3(0);
	outCollision.area = area;
	outCollision.dpdus = edge0;

	return true;
}

bool CachedTriangle::IntersectP(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(n, ray.d);
	if (fabs(NdotRayDirection) < ShadowEpsilon)
		return false;

	Float t = (d - glm::dot(n, ray.o)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax)
		return false;

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 vp0 = P - v0;
	C = glm::cross(edge0, vp0);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp1 = P - v1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(n, C) < 0)
		return false;

	Vec3 vp2 = P - v2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(n, C) < 0)
		return false;

	return true;
}

Sphere::Sphere(Vec3 _c, Float _r)
	: c(_c), r(_r) {}

Bounds3f Sphere::Bounds() const {
	return Bounds3f(Vec3(-r), Vec3(r));
}

bool Sphere::Intersect(const Ray& ray, RTInteraction& out_collision, Float tMax) const {
	Vec3 oc = ray.o - c;
	Float a = length2(ray.d);
	Float half_b = glm::dot(oc, ray.d);
	Float c = length2(oc) - r * r;

	Float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	Float sqrtd = sqrt(discriminant);

	Float root = (-half_b - sqrtd) / a;
	if (root < ShadowEpsilon || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < ShadowEpsilon || tMax < root)
			return false;
	}

	out_collision.distance = root;
	out_collision.p = ray.At(root);
	Vec3 outNormal = (out_collision.p - c) / r;
	Float theta = acos(-outNormal.y);
	Float phi = atan2(-outNormal.z, outNormal.x) + Pi;
	out_collision.uv = Vec2(phi / (2.0f * Pi), theta / Pi);
	bool front_face = glm::dot(ray.d, outNormal) < 0;
	out_collision.n = front_face ? outNormal : -outNormal;
	out_collision.backFace = !front_face;

	return true;
}

bool Sphere::IntersectP(const Ray& ray, Float tMax) const {
	Vec3 oc = ray.o - c;
	Float a = length2(ray.d);
	Float half_b = glm::dot(oc, ray.d);
	Float c = length2(oc) - r * r;

	Float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	Float sqrtd = sqrt(discriminant);

	Float root = (-half_b - sqrtd) / a;
	if (root < ShadowEpsilon || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < ShadowEpsilon || tMax < root)
			return false;
	}

	return true;
}