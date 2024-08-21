#include "pch.h"
#include "TriangleCache.h"
#include "Interaction.h"

TriangleCache::TriangleCache(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	: p0(v0.position), p1(v1.position), p2(v2.position), uv0(v0.uv), uv1(v1.uv), uv2(v2.uv) {
	edge0 = p1 - p0;
	edge1 = p2 - p1;
	edge2 = p0 - p2;
	normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
	area = glm::length(glm::cross(p1 - p0, p2 - p0)) * 0.5f;
	d = glm::dot(normal, p0);
	samplePDF = 1.0f / area;
	shadingFrame = Frame::FromZ(normal);
}

Float TriangleCache::GetSolidAngle(const Vec3& point) const {
	return SphericalTriangleArea(glm::normalize(p0 - point), glm::normalize(p1 - point), glm::normalize(p2 - point));
}

ShapeSample TriangleCache::Sample(Vec2 u) const {
	SurfaceInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.position = b[0] * p0 + b[1] * p1 + b[2] * p2;
	intr.normal = normal;
	return ShapeSample(intr, samplePDF);
}

ShapeSample TriangleCache::Sample(const SurfaceInteraction& intr, Vec2 u) const {
	Float solidAngle = GetSolidAngle(intr.position);
	if (solidAngle < MinSphericalSampleArea || solidAngle > MaxSphericalSampleArea) {
		ShapeSample ss = Sample(u);
		Vec3 wi = ss.intr.position - intr.position;
		Float distanceSquared = length2(wi);
		if (distanceSquared == 0) {
			return ShapeSample();
		}
		ss.pdf /= AbsDot(ss.intr.normal, -wi) / distanceSquared;
		wi = glm::normalize(wi);
		return ss;
	}

	Float pdf = 1;
	if (intr.normal != Vec3(0, 0, 0)) {
		Vec3 rp = intr.position;
		Vec3 wi[3] = { glm::normalize(p0 - rp), glm::normalize(p1 - rp), glm::normalize(p2 - rp) };
		std::array<Float, 4> w = std::array<Float, 4> {
			std::max<Float>(0.01f, AbsDot(intr.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(intr.normal, wi[1])),
				std::max<Float>(0.01f, AbsDot(intr.normal, wi[0])),
				std::max<Float>(0.01f, AbsDot(intr.normal, wi[2]))
		};
		u = SampleBilinear(u, w);
		pdf = BilinearPDF(u, w);
	}

	Float triPDF;
	std::array<Float, 3> b = SampleSphericalTriangle({ p0, p1, p2 }, intr.position, u, &triPDF);
	if (triPDF == 0) {
		return ShapeSample();
	}
	pdf *= triPDF;

	Vec3 p = b[0] * p0 + b[1] * p1 + b[2] * p1;
	Vec2 uvSample = b[0] * uv0 + b[1] * uv1 + b[2] * uv2;

	return ShapeSample{ intr, pdf };
}
