#pragma once
#include "RTMath.h"
#include "ComplexNumber.h"

inline Vec3 FaceForward(Vec3  n, Vec3  v) {
	return (glm::dot(n, v) < 0.) ? -n : n;
}

inline Vec3 Reflect(Vec3 wo, Vec3 n) {
	return -wo + 2 * glm::dot(wo, n) * n;
}

inline bool Refract(Vec3 wi, Vec3 n, Float eta, Float* etap, Vec3* wt) {
	Float cosTheta_i = glm::dot(n, wi);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
		n = -n;
	}

	Float sin2Theta_i = std::max<Float>(0, 1 - Sqr(cosTheta_i));
	Float sin2Theta_t = sin2Theta_i / Sqr(eta);

	if (sin2Theta_t >= 1) {
		return false;
	}

	Float cosTheta_t = std::sqrt(1 - sin2Theta_t);

	*wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * Vec3(n);
	if (etap) {
		*etap = eta;
	}

	return true;
}

inline Vec2 SampleUniformDiskConcentric(Vec2 u) {
	Vec2 uOffset = (Float)2.0f * u - Vec2(1, 1);
	if (uOffset.x == 0 && uOffset.y == 0) {
		return { 0, 0 };
	}

	Float theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}

	return r * Vec2(std::cos(theta), std::sin(theta));
}

inline Vec2 SampleUniformDiskPolar(Vec2 u) {
	Float r = std::sqrt(u[0]);
	Float theta = 2 * Pi * u[1];
	return { r * std::cos(theta), r * std::sin(theta) };
}

inline Vec3 SampleUniformHemisphere(Vec2 u) {
	Float z = u[0];
	Float r = glm::sqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline Vec3 SampleUniformSphere(Vec2 u) {
	Float z = 1 - 2 * u[0];
	Float r = SafeSqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline Vec3 SampleCosineHemisphere(Vec2 u) {
	Vec2 d = SampleUniformDiskConcentric(u);
	Float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
	return Vec3(d.x, d.y, z);
}

inline Float CosineHemispherePDF(Float cosTheta) {
	return cosTheta * InvPi;
}

inline Float SphericalTriangleArea(Vec3 a, Vec3 b, Vec3 c) {
	return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)), 1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

inline Vec3 SampleUniformTriangle(Vec2 u) {
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

inline std::array<Float, 3> SampleSphericalTriangle(const std::array<Vec3, 3>& v, Vec3 p, Vec2 u, Float* pdf) {
	if (pdf) *pdf = 0;
	Vec3 a = glm::normalize(v[0] - p), b = glm::normalize(v[1] - p), c = glm::normalize(v[2] - p);
	Vec3 n_ab = glm::cross(a, b), n_bc = glm::cross(b, c), n_ca = glm::cross(c, a);
	if (length2(n_ab) == 0 || length2(n_bc) == 0 || length2(n_ca) == 0)
		return {};
	n_ab = glm::normalize(n_ab);
	n_bc = glm::normalize(n_bc);
	n_ca = glm::normalize(n_ca);

	Float alpha = AngleBetween(n_ab, -n_ca);
	Float beta = AngleBetween(n_bc, -n_ab);
	Float gamma = AngleBetween(n_ca, -n_bc);

	Float A_pi = alpha + beta + gamma;
	Float Ap_pi = Lerp(u[0], Pi, A_pi);
	if (pdf) {
		Float A = A_pi - Pi;
		*pdf = (A <= 0) ? 0 : 1 / A;
	}

	Float cosAlpha = std::cos(alpha), sinAlpha = std::sin(alpha);
	Float sinPhi = std::sin(Ap_pi) * cosAlpha - std::cos(Ap_pi) * sinAlpha;
	Float cosPhi = std::cos(Ap_pi) * cosAlpha + std::sin(Ap_pi) * sinAlpha;
	Float k1 = cosPhi + cosAlpha;
	Float k2 = sinPhi - sinAlpha * glm::dot(a, b) /* cos c */;
	Float cosBp = (k2 + (DifferenceOfProducts(k2, cosPhi, k1, sinPhi)) * cosAlpha) / ((SumOfProducts(k2, sinPhi, k1, cosPhi)) * sinAlpha);
	cosBp = Clamp(cosBp, -1, 1);

	Float sinBp = SafeSqrt(1 - Sqr(cosBp));
	Vec3 cp = cosBp * a + sinBp * glm::normalize(GramSchmidt(c, a));

	Float cosTheta = 1 - u[1] * (1 - glm::dot(cp, b));
	Float sinTheta = SafeSqrt(1 - Sqr(cosTheta));
	Vec3 w = cosTheta * b + sinTheta * glm::normalize(GramSchmidt(cp, b));
	Vec3 e1 = v[1] - v[0], e2 = v[2] - v[0];
	Vec3 s1 = glm::cross(w, e2);
	Float divisor = glm::dot(s1, e1);
	if (divisor == 0) {
		return { 1.f / 3.f, 1.f / 3.f, 1.f / 3.f };
	}
	Float invDivisor = 1 / divisor;
	Vec3 s = p - v[0];
	Float b1 = glm::dot(s, s1) * invDivisor;
	Float b2 = glm::dot(w, glm::cross(s, e1)) * invDivisor;

	b1 = Clamp(b1, 0, 1);
	b2 = Clamp(b2, 0, 1);
	if (b1 + b2 > 1) {
		b1 /= b1 + b2;
		b2 /= b1 + b2;
	}
	return { Float(1 - b1 - b2), Float(b1), Float(b2) };
}

inline Vec2 InvertSphericalTriangleSample(const std::array<Vec3, 3>& v, Vec3 p, Vec3 w) {
	Vec3 a = glm::normalize(v[0] - p), b = glm::normalize(v[1] - p), c = glm::normalize(v[2] - p);
	Vec3 n_ab = glm::cross(a, b), n_bc = glm::cross(b, c), n_ca = glm::cross(c, a);
	if (length2(n_ab) == 0 || length2(n_bc) == 0 || length2(n_ca) == 0) {
		return Vec2();
	}
	n_ab = glm::normalize(n_ab);
	n_bc = glm::normalize(n_bc);
	n_ca = glm::normalize(n_ca);

	Float alpha = AngleBetween(n_ab, -n_ca);
	Float beta = AngleBetween(n_bc, -n_ab);
	Float gamma = AngleBetween(n_ca, -n_bc);

	Vec3 cp = glm::normalize(glm::cross(glm::cross(b, w), glm::cross(c, a)));
	if (glm::dot(cp, a + c) < 0) {
		cp = -cp;
	}

	Float u0;
	if (glm::dot(a, cp) > 0.99999847691f) {
		u0 = 0;
	}
	else {
		Vec3 n_cpb = glm::cross(cp, b), n_acp = glm::cross(a, cp);
		if (length2(n_cpb) == 0 || length2(n_acp) == 0) {
			return Vec2(0.5, 0.5);
		}
		n_cpb = glm::normalize(n_cpb);
		n_acp = glm::normalize(n_acp);
		Float Ap = alpha + AngleBetween(n_ab, n_cpb) + AngleBetween(n_acp, -n_cpb) - Pi;

		Float A = alpha + beta + gamma - Pi;
		u0 = Ap / A;
	}

	Float u1 = (1 - glm::dot(w, b)) / (1 - glm::dot(cp, b));
	return Vec2(Clamp(u0, 0, 1), Clamp(u1, 0, 1));
}

inline Float FrDielectric(Float cosTheta_i, Float eta) {
	cosTheta_i = Clamp(cosTheta_i, -1, 1);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
	}

	Float sin2Theta_i = 1 - Sqr(cosTheta_i);
	Float sin2Theta_t = sin2Theta_i / Sqr(eta);
	if (sin2Theta_t >= 1) {
		return 1.0f;
	}
	Float cosTheta_t = glm::sqrt(1 - sin2Theta_t);

	Float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	Float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

inline Float FrComplex(Float cosTheta_i, Complex<Float> eta) {
	cosTheta_i = Clamp(cosTheta_i, 0, 1);
	Float sin2Theta_i = 1 - Sqr(cosTheta_i);
	Complex<Float> sin2Theta_t = sin2Theta_i / Sqr(eta);
	Complex<Float> cosTheta_t = sqrt(1 - sin2Theta_t);

	Complex<Float> r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	Complex<Float> r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (norm(r_parl) + norm(r_perp)) / 2;
}

inline Spectrum FrComplex(Float cosTheta_i, Spectrum eta, Spectrum k) {
	return Spectrum(
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().x, k.GetRGB().x)),
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().y, k.GetRGB().y)),
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().z, k.GetRGB().z))
	);
}
