#include "RTMath.h"

float SafeSqrt(float v) {
	return glm::sqrt(glm::max(0.0f, v));
}

float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
	float f = nf * fPdf, g = ng * gPdf;
	if (std::isinf(Sqr(f)))
		return 1;
	return Sqr(f) / (Sqr(f) + Sqr(g));
}

float SphericalTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
	return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)), 1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

float MaxComponent(const glm::vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

glm::vec3 SampleUniformSphere(glm::vec2 u) {
	float z = 1 - 2 * u[0];
	float r = glm::sqrt(1 - Sqr(z));
	float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

float UniformSpherePDF() {
	return Inv4Pi;
}

glm::vec3 SampleUniformHemisphere(glm::vec2 u) {
	float z = u[0];
	float r = glm::sqrt(1 - Sqr(z));
	float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

float UniformHemispherePDF() {
	return Inv2Pi;
}

void CoordinateSystem(glm::vec3 v1, glm::vec3* v2, glm::vec3* v3) {
	float sign = std::copysign(float(1), v1.z);
	float a = -1 / (sign + v1.z);
	float b = v1.x * v1.y * a;
	*v2 = glm::vec3(1 + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
	*v3 = glm::vec3(b, sign + Sqr(v1.y) * a, -v1.y);
}

glm::vec2 SampleUniformDiskConcentric(glm::vec2 u) {
	glm::vec2 uOffset = 2.0f * u - glm::vec2(1, 1);
	if (uOffset.x == 0 && uOffset.y == 0)
		return { 0, 0 };

	float theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}
	return r * glm::vec2(std::cos(theta), std::sin(theta));
}

glm::vec2 SampleUniformDiskPolar(glm::vec2 u) {
	float r = std::sqrt(u[0]);
	float theta = 2 * Pi * u[1];
	return { r * std::cos(theta), r * std::sin(theta) };
}

bool SameHemisphere(glm::vec3 w, glm::vec3 wp) {
	return w.z * wp.z > 0;
}

glm::vec3 SampleCosineHemisphere(glm::vec2 u) {
	glm::vec2 d = SampleUniformDiskConcentric(u);
	float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
	return glm::vec3(d.x, d.y, z);
}

float CosineHemispherePDF(float cosTheta) {
	return cosTheta * InvPi;
}

float CosTheta(glm::vec3 w) {
	return w.z;
}

float Cos2Theta(glm::vec3 w) {
	return Sqr(w.z);
}

float AbsCosTheta(glm::vec3 w) {
	return std::abs(w.z);
}

float Sin2Theta(glm::vec3 w) {
	return std::max<float>(0, 1 - Cos2Theta(w));
}

float SinTheta(glm::vec3 w) {
	return std::sqrt(Sin2Theta(w));
}

float TanTheta(glm::vec3 w) {
	return SinTheta(w) / CosTheta(w);
}

float Tan2Theta(glm::vec3 w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

float CosPhi(glm::vec3 w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

float SinPhi(glm::vec3 w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

bool Refract(glm::vec3 wi, glm::vec3 n, float eta, float* etap, glm::vec3* wt) {
	float cosTheta_i = glm::dot(n, wi);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
		n = -n;
	}

	float sin2Theta_i = std::max<float>(0, 1 - Sqr(cosTheta_i));
	float sin2Theta_t = sin2Theta_i / Sqr(eta);

	if (sin2Theta_t >= 1)
		return false;

	float cosTheta_t = std::sqrt(1 - sin2Theta_t);

	*wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * glm::vec3(n);
	if (etap)
		*etap = eta;

	return true;
}

float FrDielectric(float cosTheta_i, float eta) {
	cosTheta_i = Clamp(cosTheta_i, -1, 1);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
	}

	float sin2Theta_i = 1 - Sqr(cosTheta_i);
	float sin2Theta_t = sin2Theta_i / Sqr(eta);
	if (sin2Theta_t >= 1)
		return 1.f;
	float cosTheta_t = glm::sqrt(1 - sin2Theta_t);

	float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

glm::vec3 FaceForward(glm::vec3  n, glm::vec3  v) {
	return (glm::dot(n, v) < 0.f) ? -n : n;
}

glm::vec3 Reflect(glm::vec3 wo, glm::vec3 n) {
	return -wo + 2 * glm::dot(wo, n) * n;
}

float gamma(int32_t n) {
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

bool isnan(const glm::vec3& v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}