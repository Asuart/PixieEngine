#include "pch.h"
#include "BSDF.h"

DiffuseBxDF::DiffuseBxDF(glm::vec3 R)
	: R(R) {}

glm::vec3 DiffuseBxDF::f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi))
		return glm::vec3(0.f);
	return R * InvPi;
}

std::optional<BSDFSample> DiffuseBxDF::Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
		return {};
	glm::vec3 wi = SampleCosineHemisphere(u);
	if (wo.z < 0) wi.z *= -1;
	float pdf = CosineHemispherePDF(AbsCosTheta(wi));
	if (isnan(wi)) {
		std::cout << "nan wi\n";
	}
	return BSDFSample(R * InvPi, wi, pdf, BxDFFlags::DiffuseReflection);
}

float DiffuseBxDF::PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection) ||
		!SameHemisphere(wo, wi))
		return 0;
	return CosineHemispherePDF(AbsCosTheta(wi));
}

void DiffuseBxDF::Regularize() {}

BxDFFlags DiffuseBxDF::Flags() const {
	return R != glm::vec3(0) ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset;
}

DielectricBxDF::DielectricBxDF(float eta, TrowbridgeReitzDistribution mfDistrib)
	: eta(eta), mfDistrib(mfDistrib) {}

BxDFFlags DielectricBxDF::Flags() const {
	BxDFFlags flags = BxDFFlags((eta == 1.0) ? BxDFFlags::Transmission : (BxDFFlags::Reflection | BxDFFlags::Transmission));
	return BxDFFlags(flags | (mfDistrib.EffectivelySmooth() ? BxDFFlags::Specular : BxDFFlags::Glossy));
}

std::optional<BSDFSample> DielectricBxDF::Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (eta == 1 || mfDistrib.EffectivelySmooth()) {
		float R = FrDielectric(CosTheta(wo), eta), T = 1 - R;
		float pr = R, pt = T;
		if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
			pr = 0;
		if (!((int)sampleFlags & (int)BxDFReflTransFlags::Transmission))
			pt = 0;
		if (pr == 0 && pt == 0)
			return {};

		if (uc < pr / (pr + pt)) {
			glm::vec3 wi(-wo.x, -wo.y, wo.z);
			glm::vec3 fr(R / AbsCosTheta(wi));
			return BSDFSample(fr, wi, pr / (pr + pt), BxDFFlags::SpecularReflection);
		}
		else {
			glm::vec3 wi;
			float etap;
			bool valid = Refract(wo, glm::vec3(0, 0, 1), eta, &etap, &wi);
			if (!valid)
				return {};

			glm::vec3 ft(T / AbsCosTheta(wi));
			if (mode == TransportMode::Radiance)
				ft /= Sqr(etap);

			float pdf = pt / (pr + pt);

			return BSDFSample(glm::vec3(1), wi, pdf, BxDFFlags::SpecularTransmission, etap);
		}

	}
	else {
		glm::vec3 wm = mfDistrib.Sample_wm(wo, u);
		float R = FrDielectric(glm::dot(wo, wm), eta);
		float T = 1 - R;
		float pr = R, pt = T;
		if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
			pr = 0;
		if (!((int)sampleFlags & (int)BxDFReflTransFlags::Transmission))
			pt = 0;
		if (pr == 0 && pt == 0)
			return {};

		float pdf;
		if (uc < pr / (pr + pt)) {
			glm::vec3 wi = Reflect(wo, wm);
			if (!SameHemisphere(wo, wi))
				return {};
			pdf = mfDistrib.PDF(wo, wm) / (4 * glm::abs(glm::dot(wo, wm))) * pr / (pr + pt);

			glm::vec3 f(mfDistrib.D(wm) * mfDistrib.G(wo, wi) * R / (4 * CosTheta(wi) * CosTheta(wo)));
			return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
		}
		else {
			float etap;
			glm::vec3 wi;
			bool tir = !Refract(wo, (glm::vec3)wm, eta, &etap, &wi);
			if (SameHemisphere(wo, wi) || wi.z == 0 || tir)
				return {};

			float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
			float dwm_dwi = glm::abs(glm::dot(wi, wm)) / denom;
			pdf = mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);

			glm::vec3 ft(T * mfDistrib.D(wm) * mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / (CosTheta(wi) * CosTheta(wo) * denom)));
			if (mode == TransportMode::Radiance)
				ft /= Sqr(etap);

			return BSDFSample(ft, wi, pdf, BxDFFlags::GlossyTransmission, etap);
		}
	}
}

glm::vec3 DielectricBxDF::f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const {
	if (eta == 1 || mfDistrib.EffectivelySmooth())
		return glm::vec3(0.f);

	float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	float etap = 1;
	if (!reflect)
		etap = cosTheta_o > 0 ? eta : (1 / eta);
	glm::vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || glm::length2(wm) == 0)
		return {};
	wm = FaceForward(glm::normalize(wm), glm::vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0)
		return {};

	float F = FrDielectric(glm::dot(wo, wm), eta);
	if (reflect) {
		return glm::vec3(mfDistrib.D(wm) * mfDistrib.G(wo, wi) * F / std::abs(4 * cosTheta_i * cosTheta_o));
	}
	else {
		float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap) * cosTheta_i * cosTheta_o;
		float D = mfDistrib.D(wm);
		float G = mfDistrib.G(wo, wi);
		float ft = mfDistrib.D(wm) * (1 - F) * mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / denom);

		if (mode == TransportMode::Radiance)
			ft /= Sqr(etap);

		return glm::vec3(ft);
	}
}

float DielectricBxDF::PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (eta == 1 || mfDistrib.EffectivelySmooth())
		return 0;

	float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	float etap = 1;
	if (!reflect)
		etap = cosTheta_o > 0 ? eta : (1 / eta);
	glm::vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || glm::length2(wm) == 0)
		return {};
	wm = FaceForward(glm::normalize(wm), glm::vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0)
		return {};

	float R = FrDielectric(glm::dot(wo, wm), eta);
	float T = 1 - R;

	float pr = R, pt = T;
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
		pr = 0;
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Transmission))
		pt = 0;
	if (pr == 0 && pt == 0)
		return {};

	float pdf;
	if (reflect) {
		pdf = mfDistrib.PDF(wo, wm) / (4 * glm::abs(glm::dot(wo, wm))) * pr / (pr + pt);
	}
	else {
		float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
		float dwm_dwi = glm::abs(glm::dot(wi, wm)) / denom;
		pdf = mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);
	}
	return pdf;
}

void DielectricBxDF::Regularize() {
	mfDistrib.Regularize();
}

DiffuseTransmissionBxDF::DiffuseTransmissionBxDF(glm::vec3 R, glm::vec3 T)
	: R(R), T(T) {}

glm::vec3 DiffuseTransmissionBxDF::f(glm::vec3 wo, glm::vec3 wi, TransportMode mode) const {
	return SameHemisphere(wo, wi) ? (R * InvPi) : (T * InvPi);
}

std::optional<BSDFSample> DiffuseTransmissionBxDF::Sample_f(glm::vec3 wo, float uc, glm::vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	float pr = MaxComponent(R), pt = MaxComponent(T);
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
		pr = 0;
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Transmission))
		pt = 0;
	if (pr == 0 && pt == 0)
		return {};

	if (uc < pr / (pr + pt)) {
		glm::vec3 wi = SampleCosineHemisphere(u);
		if (wo.z < 0)
			wi.z *= -1;
		float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pr / (pr + pt);
		return BSDFSample(f(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseReflection);
	}
	else {
		glm::vec3 wi = SampleCosineHemisphere(u);
		if (wo.z > 0)
			wi.z *= -1;
		float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pt / (pr + pt);
		return BSDFSample(f(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseTransmission);
	}
}

float DiffuseTransmissionBxDF::PDF(glm::vec3 wo, glm::vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	float pr = MaxComponent(R), pt = MaxComponent(T);
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Reflection))
		pr = 0;
	if (!((int)sampleFlags & (int)BxDFReflTransFlags::Transmission))
		pt = 0;
	if (pr == 0 && pt == 0)
		return {};

	if (SameHemisphere(wo, wi))
		return pr / (pr + pt) * CosineHemispherePDF(AbsCosTheta(wi));
	else
		return pt / (pr + pt) * CosineHemispherePDF(AbsCosTheta(wi));
}

void DiffuseTransmissionBxDF::Regularize() {}

BxDFFlags DiffuseTransmissionBxDF::Flags() const {
	return BxDFFlags((R == glm::vec3(0) ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset) | (T == glm::vec3(0) ? BxDFFlags::DiffuseTransmission : BxDFFlags::Unset));
}

BSDF::BSDF(glm::vec3 ns, glm::vec3 dpdus, BxDF* bxdf)
	: bxdf(bxdf), shadingFrame(Frame::FromXZ(glm::normalize(dpdus), glm::vec3(ns))) {}

BSDF::~BSDF() {
	delete bxdf;
}

BSDF::operator bool() const {
	return (bool)bxdf;
}

BxDFFlags BSDF::Flags() const {
	return bxdf->Flags();
}

glm::vec3 BSDF::RenderToLocal(glm::vec3 v) const {
	return shadingFrame.ToLocal(v);
}

glm::vec3 BSDF::LocalToRender(glm::vec3 v) const {
	return shadingFrame.FromLocal(v);
}

glm::vec3 BSDF::f(glm::vec3 woRender, glm::vec3 wiRender, TransportMode mode) const {
	glm::vec3 wi = RenderToLocal(wiRender), wo = RenderToLocal(woRender);
	if (wo.z == 0) return glm::vec3(0);
	return bxdf->f(wo, wi, mode);
}

std::optional<BSDFSample> BSDF::Sample_f(glm::vec3 woRender, float u, glm::vec2 u2, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	glm::vec3 wo = RenderToLocal(woRender);
	BxDFFlags flags = bxdf->Flags();
	if (wo.z == 0 || !(flags & (int)sampleFlags)) return {};
	std::optional<BSDFSample> bs = bxdf->Sample_f(wo, u, u2, mode, sampleFlags);
	if (!bs || bs->f == glm::vec3(0) || bs->pdf == 0 || bs->wi.z == 0)
		return {};
	bs->wi = LocalToRender(bs->wi);
	return bs;
}

float BSDF::PDF(glm::vec3 woRender, glm::vec3 wiRender, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	glm::vec3 wo = RenderToLocal(woRender), wi = RenderToLocal(wiRender);
	if (wo.z == 0) return 0;
	return bxdf->PDF(wo, wi, mode, sampleFlags);
}

glm::vec3 BSDF::rho(glm::vec3 woRender, std::span<const float> uc, std::span<const glm::vec2> u) const {
	glm::vec3 wo = RenderToLocal(woRender);
	return bxdf->rho(wo, uc, u);
}

void BSDF::Regularize() {
	bxdf->Regularize();
}