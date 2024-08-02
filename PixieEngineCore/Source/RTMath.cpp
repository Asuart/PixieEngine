#include "RTMath.h"

Float SafeSqrt(Float v) {
	return glm::sqrt(glm::max((Float)0.0f, v));
}

Float PowerHeuristic(int32_t nf, Float fPdf, int32_t ng, Float gPdf) {
	Float f = nf * fPdf, g = ng * gPdf;
	if (std::isinf(Sqr(f)))
		return 1;
	return Sqr(f) / (Sqr(f) + Sqr(g));
}

Float SphericalTriangleArea(Vec3 a, Vec3 b, Vec3 c) {
	return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)), 1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

Float MaxComponent(const Vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

Float UniformSpherePDF() {
	return Inv4Pi;
}

Float UniformHemispherePDF() {
	return Inv2Pi;
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
	Float r = glm::sqrt(1 - Sqr(z));
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

	if (sin2Theta_t >= 1)
		return false;

	Float cosTheta_t = std::sqrt(1 - sin2Theta_t);

	*wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * Vec3(n);
	if (etap)
		*etap = eta;

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