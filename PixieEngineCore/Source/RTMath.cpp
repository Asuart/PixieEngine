#include "pch.h"
#include "RTMath.h"

Float SafeSqrt(Float v) {
	return glm::sqrt(glm::max((Float)0.0f, v));
}

Float PowerHeuristic(int32_t nf, Float fPdf, int32_t ng, Float gPdf) {
	Float f = nf * fPdf, g = ng * gPdf;
	if (std::isinf(Sqr(f))) {
		return 1;
	}
	return Sqr(f) / (Sqr(f) + Sqr(g));
}

Float SphericalTriangleArea(Vec3 a, Vec3 b, Vec3 c) {
	return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)), 1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

Float MaxComponent(const Vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

Float CosineHemispherePDF(Float cosTheta) {
	return cosTheta * InvPi;
}

Float CosTheta(Vec3 w) {
	return w.z;
}

Float Cos2Theta(Vec3 w) {
	return Sqr(w.z);
}

Float AbsCosTheta(Vec3 w) {
	return std::abs(w.z);
}

Float Sin2Theta(Vec3 w) {
	return std::max<Float>(0, 1 - Cos2Theta(w));
}

Float SinTheta(Vec3 w) {
	return std::sqrt(Sin2Theta(w));
}

Float TanTheta(Vec3 w) {
	return SinTheta(w) / CosTheta(w);
}

Float Tan2Theta(Vec3 w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

Float CosPhi(Vec3 w) {
	Float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

Float SinPhi(Vec3 w) {
	Float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

Float FrDielectric(Float cosTheta_i, Float eta) {
	cosTheta_i = Clamp(cosTheta_i, -1, 1);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
	}

	Float sin2Theta_i = 1 - Sqr(cosTheta_i);
	Float sin2Theta_t = sin2Theta_i / Sqr(eta);
	if (sin2Theta_t >= 1)
		return 1.f;
	Float cosTheta_t = glm::sqrt(1 - sin2Theta_t);

	Float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	Float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

Float AbsDot(const Vec3& v1, const Vec3& v2) {
	return glm::abs(glm::dot(v1, v2));
}

Float SmoothStep(Float x, Float a, Float b) {
	if (a == b) return (x < a) ? 0 : 1;
	Float t = Clamp((x - a) / (b - a), 0, 1);
	return t * t * (3 - 2 * t);
}

Float SafeASin(Float x) {
	return std::asin(Clamp(x, -1, 1)); 
}

Float SafeACos(Float x) { 
	return std::acos(Clamp(x, -1, 1)); 
}

Float length2(const Vec2& v) {
	return v.x * v.x + v.y * v.y;
}

Float length2(const Vec3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

Float gamma(int32_t n) {
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

Vec2 SampleUniformDiskConcentric(Vec2 u) {
	Vec2 uOffset = (Float)2.0f * u - Vec2(1, 1);
	if (uOffset.x == 0 && uOffset.y == 0)
		return { 0, 0 };

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

Vec2 SampleUniformDiskPolar(Vec2 u) {
	Float r = std::sqrt(u[0]);
	Float theta = 2 * Pi * u[1];
	return { r * std::cos(theta), r * std::sin(theta) };
}

Vec3 SampleCosineHemisphere(Vec2 u) {
	Vec2 d = SampleUniformDiskConcentric(u);
	Float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
	return Vec3(d.x, d.y, z);
}

Vec3 FaceForward(Vec3  n, Vec3  v) {
	return (glm::dot(n, v) < 0.) ? -n : n;
}

Vec3 Reflect(Vec3 wo, Vec3 n) {
	return -wo + 2 * glm::dot(wo, n) * n;
}

Vec3 SampleUniformSphere(Vec2 u) {
	Float z = 1 - 2 * u[0];
	Float r = SafeSqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

Vec3 SampleUniformHemisphere(Vec2 u) {
	Float z = u[0];
	Float r = glm::sqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

bool SameHemisphere(Vec3 w, Vec3 wp) {
	return w.z * wp.z > 0;
}

bool Refract(Vec3 wi, Vec3 n, Float eta, Float* etap, Vec3* wt) {
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

bool isnan(const Vec3& v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

void CoordinateSystem(Vec3 v1, Vec3* v2, Vec3* v3) {
	Float sign = std::copysign(Float(1), v1.z);
	Float a = -1 / (sign + v1.z);
	Float b = v1.x * v1.y * a;
	*v2 = Vec3(1 + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
	*v3 = Vec3(b, sign + Sqr(v1.y) * a, -v1.y);
}

Float DifferenceOfProducts(Float a, Float b, Float c, Float d) {
	Float cd = c * d;
	Float differenceOfProducts = std::fma(a, b, -cd);
	Float error = std::fma(-c, d, cd);
	return differenceOfProducts + error;
}

Float SumOfProducts(Float a, Float b, Float c, Float d) {
	Float cd = c * d;
	Float sumOfProducts = std::fma(a, b, cd);
	Float error = std::fma(c, d, -cd);
	return sumOfProducts + error;
}

Float AngleBetween(Vec3 v1, Vec3 v2) {
	if (glm::dot(v1, v2) < 0) {
		return Pi - 2 * SafeASin(glm::length(v1 + v2) / 2);
	}
	else {
		return 2 * SafeASin(glm::length(v2 - v1) / 2);
	}
}

Float SampleLinear(Float u, Float a, Float b) {
	if (u == 0 && a == 0) {
		return 0;
	}
	Float x = u * (a + b) / (a + std::sqrt(Lerp(u, Sqr(a), Sqr(b))));
	return std::min(x, OneMinusEpsilon);
}

Vec2 SampleBilinear(Vec2 u, const std::array<Float, 4>& w) {
	Vec2 p;
	p.y = SampleLinear(u[1], w[0] + w[1], w[2] + w[3]);
	p.x = SampleLinear(u[0], Lerp(p.y, w[0], w[2]), Lerp(p.y, w[1], w[3]));
	return p;
}

Float BilinearPDF(Vec2 p, const std::array<Float, 4>& w) {
	if (p.x < 0 || p.x > 1 || p.y < 0 || p.y > 1) {
		return 0;
	}
	if (w[0] + w[1] + w[2] + w[3] == 0) {
		return 1;
	}
	return 4 * ((1 - p[0]) * (1 - p[1]) * w[0] + p[0] * (1 - p[1]) * w[1] + (1 - p[0]) * p[1] * w[2] + p[0] * p[1] * w[3]) / (w[0] + w[1] + w[2] + w[3]);
}

Vec3 GramSchmidt(Vec3 v, Vec3 w) {
	return v - glm::dot(v, w) * w;
}

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

std::array<Float, 3> SampleSphericalTriangle(const std::array<Vec3, 3>& v, Vec3 p, Vec2 u, Float* pdf) {
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

Vec2 InvertSphericalTriangleSample(const std::array<Vec3, 3>& v, Vec3 p, Vec3 w) {
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
