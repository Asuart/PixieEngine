#include "pch.h"
#include "Triangle.h"
#include "Interaction.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	: p0(v0.position), p1(v1.position), p2(v2.position), uv0(v0.uv), uv1(v1.uv), uv2(v2.uv) {
	edge0 = p1 - p0;
	edge1 = p2 - p1;
	edge2 = p0 - p2;
	normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
	area = glm::length(glm::cross(p1 - p0, p2 - p0)) * 0.5f;
	d = glm::dot(normal, p0);
	samplePDF = 1.0f / area;
}

Float Triangle::Area() const {
	return area;
}

std::optional<ShapeIntersection> Triangle::Intersect(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return {};
	}

	Float t = (d - glm::dot(normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return {};
	}

	Vec3 P = ray.At(t);

	Vec3 vp0 = P - p0;
	Vec3 C = glm::cross(edge0, vp0);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	Vec3 vp1 = P - p1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	Vec3 vp2 = P - p2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	SurfaceInteraction intr;
	intr.normal = NdotRayDirection < 0 ? -normal : normal;
	intr.position = P;
	intr.uv = Vec3(0);
	intr.dpdu = edge0;
	intr.wo = -ray.direction;

	return ShapeIntersection(intr, t);
}

bool Triangle::IsIntersected(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return false;
	}

	Float t = (d - glm::dot(normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}

	Vec3 P = ray.At(t);

	Vec3 vp0 = P - p0;
	Vec3 C = glm::cross(edge0, vp0);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	Vec3 vp1 = P - p1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	Vec3 vp2 = P - p2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	return true;
}

Bounds3f Triangle::Bounds() const {
	return Bounds3f(glm::min(p0, p1, p2), glm::max(p0, p1, p2));
}

std::optional<ShapeSample> Triangle::Sample(Vec2 u) const {
	SurfaceInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.position = b[0] * p0 + b[1] * p1 + b[2] * p2;
	intr.normal = normal;
	return ShapeSample(intr, samplePDF);
}

std::optional<ShapeSample> Triangle::Sample(const ShapeSampleContext& ctx, Vec2 u) const {
	Float solidAngle = GetSolidAngle(ctx.position);
	if (solidAngle < MinSphericalSampleArea || solidAngle > MaxSphericalSampleArea) {
		std::optional<ShapeSample> ss = Sample(u);
		Vec3 wi = ss->intr.position - ctx.position;
		Float distanceSquared = length2(wi);
		if (distanceSquared == 0) {
			return {};
		}
		wi = glm::normalize(wi);
		ss->pdf /= AbsDot(ss->intr.normal, -wi) / distanceSquared;
		if (ss->pdf == Infinity) {
			return {};
		}
		return ss;
	}

	Float pdf = 1;
	if (ctx.normal != Vec3(0, 0, 0)) {
		Vec3 rp = ctx.position;
		Vec3 wi[3] = { glm::normalize(p0 - rp), glm::normalize(p1 - rp), glm::normalize(p2 - rp) };
		std::array<Float, 4> w = std::array<Float, 4> {
			std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[0])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[2]))
		};
		u = SampleBilinear(u, w);
		pdf = BilinearPDF(u, w);
	}

	Float triPDF;
	std::array<Float, 3> b = SampleSphericalTriangle({ p0, p1, p2 }, ctx.position, u, &triPDF);
	if (triPDF == 0) {
		return {};
	}
	pdf *= triPDF;

	Vec3 p = b[0] * p0 + b[1] * p1 + b[2] * p1;
	Vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));
	Vec2 uvSample = b[0] * uv0 + b[1] * uv1 + b[2] * uv2;

	return ShapeSample(SurfaceInteraction(p, n, uvSample), pdf);
}

Float Triangle::SamplePDF(const SurfaceInteraction&) const {
	return 1.0f / area;
}

Float Triangle::SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const {
	Float solidAngle = GetSolidAngle(ctx.position);
	if (solidAngle < MinSphericalSampleArea || solidAngle > MaxSphericalSampleArea) {
		Ray ray = Ray(ctx.position, wi);
		std::optional<ShapeIntersection> isect = Intersect(ray);
		if (!isect) {
			return 0;
		}
		Float pdf = (1 / Area()) / (AbsDot(isect->intr.normal, -wi) / length2(ctx.position - isect->intr.position));
		if (pdf == Infinity) {
			pdf = 0;
		}
		return pdf;
	}

	Float pdf = 1.0f / solidAngle;
	if (ctx.normal != Vec3(0.0f)) {
		Vec2 u = InvertSphericalTriangleSample({ p0, p1, p2 }, ctx.position, wi);
		Vec3 wi[3] = { glm::normalize(p0 - ctx.position), glm::normalize(p1 - ctx.position), glm::normalize(p2 - ctx.position) };
		std::array<Float, 4> w = std::array<Float, 4>{
			std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[0])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[2]))};

		pdf *= BilinearPDF(u, w);
	}

	return pdf;
}

Float Triangle::GetSolidAngle(const Vec3& point) const {
	return SphericalTriangleArea(glm::normalize(p0 - point), glm::normalize(p1 - point), glm::normalize(p2 - point));
}

CompactTriangle::CompactTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	: p0(v0.position), p1(v1.position), p2(v2.position), uv0(v0.uv), uv1(v1.uv), uv2(v2.uv) {
	normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
}

Float CompactTriangle::Area() const {
	return glm::length(glm::cross(p1 - p0, p2 - p0)) * 0.5f;;
}

std::optional<ShapeIntersection> CompactTriangle::Intersect(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return {};
	}

	Float d = glm::dot(normal, p0);

	Float t = (d - glm::dot(normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return {};
	}

	Vec3 edge0 = p1 - p0;
	Vec3 edge1 = p2 - p1;
	Vec3 edge2 = p0 - p2;

	Vec3 P = ray.At(t);

	Vec3 vp0 = P - p0;
	Vec3 C = glm::cross(edge0, vp0);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	Vec3 vp1 = P - p1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	Vec3 vp2 = P - p2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(normal, C) < 0) {
		return {};
	}

	SurfaceInteraction intr;
	intr.normal = NdotRayDirection < 0 ? -normal : normal;
	intr.position = P;
	intr.uv = Vec3(0);
	intr.dpdu = edge0;
	intr.wo = -ray.direction;

	return ShapeIntersection(intr, t);
}

bool CompactTriangle::IsIntersected(const Ray& ray, Float tMax) const {
	Float NdotRayDirection = glm::dot(normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return false;
	}

	Float d = glm::dot(normal, p0);

	Float t = (d - glm::dot(normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}

	Vec3 edge0 = p1 - p0;
	Vec3 edge1 = p2 - p1;
	Vec3 edge2 = p0 - p2;

	Vec3 P = ray.At(t);

	Vec3 vp0 = P - p0;
	Vec3 C = glm::cross(edge0, vp0);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	Vec3 vp1 = P - p1;
	C = glm::cross(edge1, vp1);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	Vec3 vp2 = P - p2;
	C = glm::cross(edge2, vp2);
	if (glm::dot(normal, C) < 0) {
		return false;
	}

	return true;
}

Bounds3f CompactTriangle::Bounds() const {
	return Bounds3f(glm::min(p0, p1, p2), glm::max(p0, p1, p2));
}

std::optional<ShapeSample> CompactTriangle::Sample(Vec2 u) const {
	SurfaceInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.position = b[0] * p0 + b[1] * p1 + b[2] * p2;
	intr.normal = normal;
	return ShapeSample(intr, SamplePDF(intr));
}

std::optional<ShapeSample> CompactTriangle::Sample(const ShapeSampleContext& ctx, Vec2 u) const {
	Float solidAngle = GetSolidAngle(ctx.position);
	if (solidAngle < MinSphericalSampleArea || solidAngle > MaxSphericalSampleArea) {
		std::optional<ShapeSample> ss = Sample(u);
		Vec3 wi = ss->intr.position - ctx.position;
		Float distanceSquared = length2(wi);
		if (distanceSquared == 0) {
			return {};
		}
		wi = glm::normalize(wi);
		ss->pdf /= AbsDot(ss->intr.normal, -wi) / distanceSquared;
		if (ss->pdf == Infinity) {
			return {};
		}
		return ss;
	}

	Float pdf = 1;
	if (ctx.normal != Vec3(0, 0, 0)) {
		Vec3 rp = ctx.position;
		Vec3 wi[3] = { glm::normalize(p0 - rp), glm::normalize(p1 - rp), glm::normalize(p2 - rp) };
		std::array<Float, 4> w = std::array<Float, 4> {
			std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[0])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[2]))
		};
		u = SampleBilinear(u, w);
		pdf = BilinearPDF(u, w);
	}

	Float triPDF;
	std::array<Float, 3> b = SampleSphericalTriangle({ p0, p1, p2 }, ctx.position, u, &triPDF);
	if (triPDF == 0) {
		return {};
	}
	pdf *= triPDF;

	Vec3 p = b[0] * p0 + b[1] * p1 + b[2] * p1;
	Vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));
	Vec2 uvSample = b[0] * uv0 + b[1] * uv1 + b[2] * uv2;

	return ShapeSample(SurfaceInteraction(p, n, uvSample), pdf);
}

Float CompactTriangle::SamplePDF(const SurfaceInteraction&) const {
	return 1.0f / Area();
}

Float CompactTriangle::SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const {
	Float solidAngle = GetSolidAngle(ctx.position);
	if (solidAngle < MinSphericalSampleArea || solidAngle > MaxSphericalSampleArea) {
		Ray ray = Ray(ctx.position, wi);
		std::optional<ShapeIntersection> isect = Intersect(ray);
		if (!isect) {
			return 0;
		}
		Float pdf = (1 / Area()) / (AbsDot(isect->intr.normal, -wi) / length2(ctx.position - isect->intr.position));
		if (pdf == Infinity) {
			pdf = 0;
		}
		return pdf;
	}

	Float pdf = 1.0f / solidAngle;
	if (ctx.normal != Vec3(0.0f)) {
		Vec2 u = InvertSphericalTriangleSample({ p0, p1, p2 }, ctx.position, wi);
		Vec3 wi[3] = { glm::normalize(p0 - ctx.position), glm::normalize(p1 - ctx.position), glm::normalize(p2 - ctx.position) };
		std::array<Float, 4> w = std::array<Float, 4>{
			std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[0])),
				std::max<Float>(0.01f, AbsDot(ctx.normal, wi[2]))};

		pdf *= BilinearPDF(u, w);
	}

	return pdf;
}

Float CompactTriangle::GetSolidAngle(const Vec3& point) const {
	return SphericalTriangleArea(glm::normalize(p0 - point), glm::normalize(p1 - point), glm::normalize(p2 - point));
}
