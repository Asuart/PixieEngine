#include "BSDF.h"

DiffuseBxDF::DiffuseBxDF(Spectrum spectrum)
	: m_spectrum(spectrum) {}

Spectrum DiffuseBxDF::f(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi)) {
		return Spectrum();
	}
	return m_spectrum * InvPi;
}

std::optional<BSDFSample> DiffuseBxDF::Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection)) {
		return {};
	}
	Vec3 wi = SampleCosineHemisphere(u);
	if (wo.z < 0) wi.z *= -1;
	Float pdf = CosineHemispherePDF(AbsCosTheta(wi));
	if (isnan(wi)) {
		std::cout << "nan wi\n";
	}
	return BSDFSample(m_spectrum * InvPi, wi, pdf, BxDFFlags::DiffuseReflection);
}

Float DiffuseBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection) || !SameHemisphere(wo, wi)) {
		return 0;
	}
	return CosineHemispherePDF(AbsCosTheta(wi));
}

void DiffuseBxDF::Regularize() {}

BxDFFlags DiffuseBxDF::Flags() const {
	return m_spectrum ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset;
}

DielectricBxDF::DielectricBxDF(Float refraction, TrowbridgeReitzDistribution mfDistrib)
	: m_refraction(refraction), m_mfDistrib(mfDistrib) {}

BxDFFlags DielectricBxDF::Flags() const {
	BxDFFlags flags = BxDFFlags((m_refraction == 1.0) ? BxDFFlags::Transmission : (BxDFFlags::Reflection | BxDFFlags::Transmission));
	return BxDFFlags(flags | (m_mfDistrib.EffectivelySmooth() ? BxDFFlags::Specular : BxDFFlags::Glossy));
}

std::optional<BSDFSample> DielectricBxDF::Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth()) {
		Float R = FrDielectric(CosTheta(wo), m_refraction), T = 1 - R;
		Float pr = R, pt = T;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection))
			pr = 0;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission))
			pt = 0;
		if (pr == 0 && pt == 0)
			return {};

		if (uc < pr / (pr + pt)) {
			Vec3 wi(-wo.x, -wo.y, wo.z);
			Vec3 fr(R / AbsCosTheta(wi));
			return BSDFSample(fr, wi, pr / (pr + pt), BxDFFlags::SpecularReflection);
		}
		else {
			Vec3 wi;
			Float etap;
			bool valid = Refract(wo, Vec3(0, 0, 1), m_refraction, &etap, &wi);
			if (!valid)
				return {};

			Vec3 ft(T / AbsCosTheta(wi));
			if (mode == TransportMode::Radiance)
				ft /= Sqr(etap);

			Float pdf = pt / (pr + pt);

			return BSDFSample(Vec3(1), wi, pdf, BxDFFlags::SpecularTransmission, etap);
		}

	}
	else {
		Vec3 wm = m_mfDistrib.Sample_wm(wo, u);
		Float R = FrDielectric(glm::dot(wo, wm), m_refraction);
		Float T = 1.0f - R;
		Float pr = R, pt = T;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection))
			pr = 0;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission))
			pt = 0;
		if (pr == 0 && pt == 0)
			return {};

		Float pdf;
		if (uc < pr / (pr + pt)) {
			Vec3 wi = Reflect(wo, wm);
			if (!SameHemisphere(wo, wi))
				return {};
			pdf = m_mfDistrib.PDF(wo, wm) / (4 * glm::abs(glm::dot(wo, wm))) * pr / (pr + pt);

			Vec3 f(m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * R / (4 * CosTheta(wi) * CosTheta(wo)));
			return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
		}
		else {
			Float etap;
			Vec3 wi;
			bool tir = !Refract(wo, (Vec3)wm, m_refraction, &etap, &wi);
			if (SameHemisphere(wo, wi) || wi.z == 0 || tir)
				return {};

			Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
			Float dwm_dwi = glm::abs(glm::dot(wi, wm)) / denom;
			pdf = m_mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);

			Vec3 ft(T * m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / (CosTheta(wi) * CosTheta(wo) * denom)));
			if (mode == TransportMode::Radiance)
				ft /= Sqr(etap);

			return BSDFSample(ft, wi, pdf, BxDFFlags::GlossyTransmission, etap);
		}
	}
}

Spectrum DielectricBxDF::f(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth()) {
		return Spectrum();
	}

	Float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	Float etap = 1;
	if (!reflect)
		etap = cosTheta_o > 0 ? m_refraction : (1 / m_refraction);
	Vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || length2(wm) == 0)
		return {};
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0)
		return {};

	Float F = FrDielectric(glm::dot(wo, wm), m_refraction);
	if (reflect) {
		return Vec3(m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * F / std::abs(4 * cosTheta_i * cosTheta_o));
	}
	else {
		Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap) * cosTheta_i * cosTheta_o;
		Float D = m_mfDistrib.D(wm);
		Float G = m_mfDistrib.G(wo, wi);
		Float ft = m_mfDistrib.D(wm) * (1 - F) * m_mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / denom);

		if (mode == TransportMode::Radiance)
			ft /= Sqr(etap);

		return Vec3(ft);
	}
}

Float DielectricBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth())
		return 0;

	Float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	Float etap = 1;
	if (!reflect)
		etap = cosTheta_o > 0 ? m_refraction : (1 / m_refraction);
	Vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || length2(wm) == 0)
		return {};
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0)
		return {};

	Float R = FrDielectric(glm::dot(wo, wm), m_refraction);
	Float T = 1 - R;

	Float pr = R, pt = T;
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection))
		pr = 0;
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission))
		pt = 0;
	if (pr == 0 && pt == 0)
		return {};

	Float pdf;
	if (reflect) {
		pdf = m_mfDistrib.PDF(wo, wm) / (4 * glm::abs(glm::dot(wo, wm))) * pr / (pr + pt);
	}
	else {
		Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
		Float dwm_dwi = glm::abs(glm::dot(wi, wm)) / denom;
		pdf = m_mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);
	}
	return pdf;
}

void DielectricBxDF::Regularize() {
	m_mfDistrib.Regularize();
}

DiffuseTransmissionBxDF::DiffuseTransmissionBxDF(Spectrum diffusionSpectrum, Spectrum transmissionSpectrum)
	: m_diffusionSpectrum(diffusionSpectrum), m_transmissionSpectrum(transmissionSpectrum) {}

Spectrum DiffuseTransmissionBxDF::f(Vec3 wo, Vec3 wi, TransportMode mode) const {
	return SameHemisphere(wo, wi) ? (m_diffusionSpectrum * InvPi) : (m_transmissionSpectrum * InvPi);
}

std::optional<BSDFSample> DiffuseTransmissionBxDF::Sample_f(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Float pr = MaxComponent(m_diffusionSpectrum.GetRGBValue()), pt = MaxComponent(m_transmissionSpectrum.GetRGBValue());
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection))
		pr = 0;
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission))
		pt = 0;
	if (pr == 0 && pt == 0)
		return {};

	if (uc < pr / (pr + pt)) {
		Vec3 wi = SampleCosineHemisphere(u);
		if (wo.z < 0)
			wi.z *= -1;
		Float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pr / (pr + pt);
		return BSDFSample(f(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseReflection);
	}
	else {
		Vec3 wi = SampleCosineHemisphere(u);
		if (wo.z > 0)
			wi.z *= -1;
		Float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pt / (pr + pt);
		return BSDFSample(f(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseTransmission);
	}
}

Float DiffuseTransmissionBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Float pr = MaxComponent(m_diffusionSpectrum.GetRGBValue()), pt = MaxComponent(m_transmissionSpectrum.GetRGBValue());
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
	return BxDFFlags((!m_diffusionSpectrum ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset) | (!m_transmissionSpectrum ? BxDFFlags::DiffuseTransmission : BxDFFlags::Unset));
}

BSDF::BSDF(Vec3 ns, Vec3 dpdus, BxDF* bxdf)
	: bxdf(bxdf), shadingFrame(Frame::FromXZ(glm::normalize(dpdus), Vec3(ns))) {}

BSDF::~BSDF() {
	delete bxdf;
}

BSDF::operator bool() const {
	return (bool)bxdf;
}

BxDFFlags BSDF::Flags() const {
	return bxdf->Flags();
}

Vec3 BSDF::RenderToLocal(Vec3 v) const {
	return shadingFrame.ToLocal(v);
}

Vec3 BSDF::LocalToRender(Vec3 v) const {
	return shadingFrame.FromLocal(v);
}

Spectrum BSDF::f(Vec3 woRender, Vec3 wiRender, TransportMode mode) const {
	Vec3 wi = RenderToLocal(wiRender), wo = RenderToLocal(woRender);
	if (wo.z == 0) return Vec3(0);
	return bxdf->f(wo, wi, mode);
}

std::optional<BSDFSample> BSDF::Sample_f(Vec3 woRender, Float u, Vec2 u2, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Vec3 wo = RenderToLocal(woRender);
	BxDFFlags flags = bxdf->Flags();
	if (wo.z == 0 || !(flags & (int)sampleFlags)) return {};
	std::optional<BSDFSample> bs = bxdf->Sample_f(wo, u, u2, mode, sampleFlags);
	if (!bs || !bs->f || bs->pdf == 0 || bs->wi.z == 0)
		return {};
	bs->wi = LocalToRender(bs->wi);
	return bs;
}

Float BSDF::PDF(Vec3 woRender, Vec3 wiRender, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Vec3 wo = RenderToLocal(woRender), wi = RenderToLocal(wiRender);
	if (wo.z == 0) return 0;
	return bxdf->PDF(wo, wi, mode, sampleFlags);
}

Spectrum BSDF::rho(Vec3 woRender, std::span<const Float> uc, std::span<const Vec2> u) const {
	Vec3 wo = RenderToLocal(woRender);
	return bxdf->rho(wo, uc, u);
}

void BSDF::Regularize() {
	bxdf->Regularize();
}