#include "pch.h"
#include "Shapes.h"

/*
	Sphere
*/

Sphere::Sphere(int32_t material, const Transform& transform, Float r) :
	Shape(material), m_transform(transform), m_radius(r) {}

Float Sphere::Area() const {
	return 4 * Pi * m_radius;
}

std::optional<ShapeIntersection> Sphere::Intersect(const Ray& ray, Float tMax) const {
	Vec3 center = m_transform.GetPosition();
	Vec3 oc = center - ray.origin;
	Float a = glm::dot(ray.direction, ray.direction);
	Float b = -2.0f * glm::dot(ray.direction, oc);
	Float c = glm::dot(oc, oc) - m_radius * m_radius;
	Float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return {};
	}
	Float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
	if (t < ShadowEpsilon || t > tMax) {
		return {};
	}
	Vec3 pHit = ray.At(t);
	Vec3 normal = glm::normalize(pHit - center);
	RayInteraction inter(pHit, normal, { 0.0f, 0.0f }, -ray.direction);
	inter.materialIndex = m_materialIndex;
	inter.lightIndex = m_lightIndex;
	return ShapeIntersection{ inter, t };
}

bool Sphere::IsIntersected(const Ray& ray, Float tMax) const {
	Vec3 center = m_transform.GetPosition();
	Vec3 oc = center - ray.origin;
	Float a = glm::dot(ray.direction, ray.direction);
	Float b = -2.0f * glm::dot(ray.direction, oc);
	Float c = glm::dot(oc, oc) - m_radius * m_radius;
	Float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return false;
	}
	Float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}
	return true;
}

Bounds3f Sphere::Bounds() const {
	return m_transform.ApplyBounds(Bounds3f(Vec3(-m_radius), Vec3(m_radius)));
}

std::optional<ShapeSample> Sphere::Sample(Vec2 u) const {
	Vec3 pObj = m_radius * glm::normalize(SampleUniformSphere(u));
	Vec3 nObj(pObj.x, pObj.y, pObj.z);
	Vec3 n = glm::normalize(m_transform.ApplyNormal(nObj));
	Float theta = SafeACos(pObj.z / m_radius);
	Float phi = std::atan2(pObj.y, pObj.x);
	if (phi < 0) {
		phi += 2 * Pi;
	}
	Vec2 uv = Vec2(phi / TwoPi, theta / TwoPi);
	Vec3 pi = m_transform.ApplyPoint(pObj);
	return ShapeSample{ RayInteraction(pi, n, uv), 1 / Area() };
}

std::optional<ShapeSample> Sphere::Sample(const ShapeSampleContext& ctx, Vec2 u) const {
	Vec3 pCenter = m_transform.ApplyPoint(Vec3(0, 0, 0));
	Vec3 pOrigin = ctx.position;
	if (length2(pOrigin - pCenter) <= Sqr(m_radius)) {
		std::optional<ShapeSample> ss = Sample(u);
		Vec3 wi = ss->intr.position - ctx.position;
		if (length2(wi) == 0) {
			return {};
		}
		wi = glm::normalize(wi);
		ss->pdf /= AbsDot(ss->intr.normal, -wi) / length2(ctx.position - ss->intr.position);
		if (ss->pdf == Infinity) {
			return {};
		}
		return ss;
	}

	Float sinThetaMax = m_radius / glm::length(ctx.position - pCenter);
	Float sin2ThetaMax = Sqr(sinThetaMax);
	Float cosThetaMax = SafeSqrt(1 - sin2ThetaMax);
	Float oneMinusCosThetaMax = 1 - cosThetaMax;

	Float cosTheta = (cosThetaMax - 1) * u[0] + 1;
	Float sin2Theta = 1 - Sqr(cosTheta);
	if (sin2ThetaMax < 0.00068523f) {
		sin2Theta = sin2ThetaMax * u[0];
		cosTheta = std::sqrt(1 - sin2Theta);
		oneMinusCosThetaMax = sin2ThetaMax / 2;
	}

	Float cosAlpha = sin2Theta / sinThetaMax + cosTheta * SafeSqrt(1 - sin2Theta / Sqr(sinThetaMax));
	Float sinAlpha = SafeSqrt(1 - Sqr(cosAlpha));

	Float phi = u[1] * 2 * Pi;
	Vec3 w = SphericalDirection(sinAlpha, cosAlpha, phi);
	Frame samplingFrame = Frame::FromZ(glm::normalize(pCenter - ctx.position));
	Vec3 n(samplingFrame.FromLocal(-w));
	Vec3 p = pCenter + m_radius * Vec3(n.x, n.y, n.z);

	Vec3 pObj = m_transform.ApplyPoint(p);
	Float theta = SafeACos(pObj.z / m_radius);
	Float spherePhi = std::atan2(pObj.y, pObj.x);
	if (spherePhi < 0) {
		spherePhi += 2 * Pi;
	}
	Vec2 uv = Vec2(spherePhi / TwoPi, theta / TwoPi);

	return ShapeSample{ RayInteraction(p, n, uv), 1 / (2 * Pi * oneMinusCosThetaMax) };
}

Float Sphere::SamplePDF(const RayInteraction&) const {
	return 1 / Area();
}

Float Sphere::SamplePDF(const ShapeSampleContext& ctx, Vec3 wi) const {
	Vec3 pCenter = m_transform.ApplyPoint(Vec3(0, 0, 0));
	Vec3 pOrigin = ctx.position;
	if (length2(pOrigin - pCenter) <= Sqr(m_radius)) {
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
	Float sin2ThetaMax = Sqr(m_radius) / length2(ctx.position - pCenter);
	Float cosThetaMax = SafeSqrt(1 - sin2ThetaMax);
	Float oneMinusCosThetaMax = 1 - cosThetaMax;
	if (sin2ThetaMax < 0.00068523f) {
		oneMinusCosThetaMax = sin2ThetaMax / 2;
	}
	return 1 / (2 * Pi * oneMinusCosThetaMax);
}

/*
	Triangle
*/

Triangle::Triangle(int32_t materialIndex, const Vertex& v0, const Vertex& v1, const Vertex& v2) :
	Shape(materialIndex), p0(v0.position), p1(v1.position), p2(v2.position), uv0(v0.uv), uv1(v1.uv), uv2(v2.uv) {
	normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
}

Float Triangle::Area() const {
	return glm::length(glm::cross(p1 - p0, p2 - p0)) * 0.5f;;
}

std::optional<ShapeIntersection> Triangle::Intersect(const Ray& ray, Float tMax) const {
	Vec3 edge1 = p1 - p0;
	Vec3 edge2 = p2 - p0;
	Vec3 ray_cross_e2 = cross(ray.direction, edge2);
	Float det = dot(edge1, ray_cross_e2);

	if (abs(det) < ShadowEpsilon) {
		return {};
	}

	Float inv_det = 1.0f / det;
	Vec3 s = ray.origin - p0;
	Float u = inv_det * dot(s, ray_cross_e2);

	if (u < 0.0f || u > 1.0f) {
		return {};
	}

	Vec3 s_cross_e1 = cross(s, edge1);
	Float v = inv_det * dot(ray.direction, s_cross_e1);

	if (v < 0.0f || u + v > 1.0f) {
		return {};
	}

	Float t = inv_det * dot(edge2, s_cross_e1);

	if (t < ShadowEpsilon || t > tMax) {
		return {};
	}

	RayInteraction intr;
	intr.normal = glm::dot(normal, ray.direction) < 0 ? normal : -normal;
	intr.position = ray.origin + ray.direction * t;
	intr.uv = Vec3(0);
	intr.wo = -ray.direction;
	intr.materialIndex = m_materialIndex;
	intr.lightIndex = m_lightIndex;

	return ShapeIntersection(intr, t);
}

bool Triangle::IsIntersected(const Ray& ray, Float tMax) const {
	Vec3 edge1 = p1 - p0;
	Vec3 edge2 = p2 - p0;
	Vec3 ray_cross_e2 = cross(ray.direction, edge2);
	Float det = dot(edge1, ray_cross_e2);

	if (abs(det) < ShadowEpsilon) {
		return false;
	}

	Float inv_det = 1.0f / det;
	Vec3 s = ray.origin - p0;
	Float u = inv_det * dot(s, ray_cross_e2);

	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	Vec3 s_cross_e1 = cross(s, edge1);
	Float v = inv_det * dot(ray.direction, s_cross_e1);

	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	Float t = inv_det * dot(edge2, s_cross_e1);

	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}

	return true;
}

Bounds3f Triangle::Bounds() const {
	return Bounds3f(glm::min(p0, p1, p2), glm::max(p0, p1, p2));
}

std::optional<ShapeSample> Triangle::Sample(Vec2 u) const {
	RayInteraction intr;
	Vec3 b = SampleUniformTriangle(u);
	intr.position = b[0] * p0 + b[1] * p1 + b[2] * p2;
	intr.normal = normal;
	return ShapeSample(intr, SamplePDF(intr));
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

	return ShapeSample(RayInteraction(p, n, uvSample), pdf);
}

Float Triangle::SamplePDF(const RayInteraction&) const {
	return 1.0f / Area();
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
