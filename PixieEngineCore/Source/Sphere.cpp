#include "pch.h"
#include "Sphere.h"

Sphere::Sphere(const Transform& transform, Float r)
	: Shape(transform), m_radius(r) {}

Float Sphere::Area() const {
	return 4 * Pi * m_radius;
}

std::optional<ShapeIntersection> Sphere::Intersect(const Ray& ray, Float tMax) const {
	Vec3 oi = m_transform.ApplyPoint(ray.origin);
	Vec3 di = m_transform.ApplyVector(ray.direction);

	Float t0, t1;
	Float a = Sqr(di.x) + Sqr(di.y) + Sqr(di.z);
	Float b = 2 * (di.x * oi.x + di.y * oi.y + di.z * oi.z);
	Float c = Sqr(oi.x) + Sqr(oi.y) + Sqr(oi.z) - Sqr(m_radius);

	Vec3 v = Vec3(oi - b / (2 * a) * di);
	Float length = glm::length(v);
	Float discrim = 4 * a * (m_radius + length) * (m_radius - length);
	if (discrim < 0) {
		return {};
	}

	Float rootDiscrim = glm::sqrt(discrim);
	Float q;
	if (b < 0) {
		q = -0.5f * (b - rootDiscrim);
	}
	else {
		q = -0.5f * (b + rootDiscrim);
	}

	t0 = q / a;
	t1 = c / q;
	if (t0 > t1) {
		std::swap(t0, t1);
	}

	if (t0 > tMax || t1 <= 0) {
		return {};
	}

	Float tShapeHit = t0;
	if (tShapeHit <= 0) {
		tShapeHit = t1;
		if (tShapeHit > tMax) {
			return {};
		}
	}

	Vec3 pHit = oi + tShapeHit * di;
	pHit *= m_radius / glm::length(pHit);

	if (pHit.x == 0 && pHit.y == 0) {
		pHit.x = 1e-5f * m_radius;
	}

	Float phi = std::atan2(pHit.y, pHit.x);
	if (phi < 0) {
		phi += 2 * Pi;
	}

	Float cosTheta = pHit.z / m_radius;
	Float theta = SafeACos(cosTheta);
	Vec2 uv = Vec2(phi / TwoPi, theta / TwoPi);

	Float zRadius = std::sqrt(Sqr(pHit.x) + Sqr(pHit.y));
	Float cosPhi = pHit.x / zRadius, sinPhi = pHit.y / zRadius;
	Vec3 dpdu = Vec3(-TwoPi * pHit.y, TwoPi * pHit.x, 0);
	Float sinTheta = SafeSqrt(1 - Sqr(cosTheta));
	Vec3 dpdv = TwoPi * Vec3(pHit.z * cosPhi, pHit.z * sinPhi, -m_radius * sinTheta);

	Vec3 d2Pduu = -Sqr(TwoPi) * Vec3(pHit.x, pHit.y, 0);
	Vec3 d2Pduv = TwoPi * pHit.z * TwoPi * Vec3(-sinPhi, cosPhi, 0.);
	Vec3 d2Pdvv = -Sqr(TwoPi) * Vec3(pHit.x, pHit.y, pHit.z);

	Float E = glm::dot(dpdu, dpdu), F = glm::dot(dpdu, dpdv), G = glm::dot(dpdv, dpdv);
	Vec3 n = glm::normalize(glm::cross(dpdu, dpdv));
	Float e = glm::dot(n, d2Pduu), f = glm::dot(n, d2Pduv), g = glm::dot(n, d2Pdvv);

	Float EGF2 = DifferenceOfProducts(E, G, F, F);
	Float invEGF2 = (EGF2 == 0) ? Float(0) : 1 / EGF2;
	Vec3 dndu = Vec3((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
	Vec3 dndv = Vec3((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

	Vec3 woObject = m_transform.ApplyInverseVector(-ray.direction);
	return ShapeIntersection{ m_transform.ApplyInteraction(SurfaceInteraction(pHit, uv, woObject, dpdu, dpdv, dndu, dndv)), tShapeHit };
}

bool Sphere::IntersectP(const Ray& ray, Float tMax) const {
	Vec3 oi = m_transform.ApplyInversePoint(ray.origin);
	Vec3 di = m_transform.ApplyInverseVector(ray.direction);

	Float t0, t1;
	Float a = Sqr(di.x) + Sqr(di.y) + Sqr(di.z);
	Float b = 2 * (di.x * oi.x + di.y * oi.y + di.z * oi.z);
	Float c = Sqr(oi.x) + Sqr(oi.y) + Sqr(oi.z) - Sqr(m_radius);

	Vec3 v = Vec3(oi - b / (2 * a) * di);
	Float length = glm::length(v);
	Float discrim = 4 * a * (m_radius + length) * (m_radius - length);
	if (discrim < 0) {
		return false;
	}

	Float rootDiscrim = glm::sqrt(discrim);
	Float q;
	if (b < 0) {
		q = -0.5f * (b - rootDiscrim);
	}
	else {
		q = -0.5f * (b + rootDiscrim);
	}

	t0 = q / a;
	t1 = c / q;
	if (t0 > t1) {
		std::swap(t0, t1);
	}

	if (t0 <= ShadowEpsilon || t1 > tMax) {
		return false;
	}

	return true;
}

Bounds3f Sphere::Bounds() const {
	return m_transform.ApplyBounds(Bounds3f(Vec3(-m_radius, -m_radius, -m_radius), Vec3(m_radius, m_radius, m_radius)));
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
	return ShapeSample{ SurfaceInteraction(pi, n, uv), 1 / Area() };
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

	return ShapeSample{ SurfaceInteraction(p, n, uv), 1 / (2 * Pi * oneMinusCosThetaMax) };
}

Float Sphere::SamplePDF(const SurfaceInteraction&) const {
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
