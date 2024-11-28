#include "pch.h"
#include "BSDF.h"
#include "Material.h"

bool IsReflective(BxDFFlags f) {
	return f & BxDFFlags::Reflection;
}

bool IsTransmissive(BxDFFlags f) {
	return f & BxDFFlags::Transmission;
}

bool IsDiffuse(BxDFFlags f) {
	return f & BxDFFlags::Diffuse;
}

bool IsGlossy(BxDFFlags f) {
	return f & BxDFFlags::Glossy;
}

bool IsSpecular(BxDFFlags f) {
	return f & BxDFFlags::Specular;
}

bool IsNonSpecular(BxDFFlags f) {
	return f & (BxDFFlags::Diffuse | BxDFFlags::Glossy);
}

/*
	BSDF Sample
*/

BSDFSample::BSDFSample(Spectrum f, Vec3 wi, Float pdf, BxDFFlags flags, Float eta, bool pdfIsProportional) :
	f(f), wi(wi), pdf(pdf), flags(flags), eta(eta), pdfIsProportional(pdfIsProportional) {}

bool BSDFSample::IsReflection() const {
	return ::IsReflective(flags);
}

bool BSDFSample::IsTransmission() const {
	return ::IsTransmissive(flags);
}

bool BSDFSample::IsDiffuse() const {
	return ::IsDiffuse(flags);
}

bool BSDFSample::IsGlossy() const {
	return ::IsGlossy(flags);
}

bool BSDFSample::IsSpecular() const {
	return ::IsSpecular(flags);
}

BSDFSample::operator bool() const {
	return pdf && f;
}

/*
	BxDF
*/

Spectrum BxDF::rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u2) const {
	Vec3 r = Vec3(0.0f);
	for (size_t i = 0; i < uc.size(); i++) {
		std::optional<BSDFSample> bs = SampleDirectionAndDistribution(wo, uc[i], u2[i]);
		if (bs && bs->pdf > 0.0f) {
			r += bs->f * AbsCosTheta(bs->wi) / bs->pdf;
		}
	}
	return Spectrum(r / (Float)uc.size());
}

/*
	Diffuse BxDF
*/

DiffuseBxDF::DiffuseBxDF(Spectrum spectrum) :
	m_spectrum(spectrum) {}

Spectrum DiffuseBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi)) {
		return Spectrum();
	}
	return m_spectrum * InvPi;
}

std::optional<BSDFSample> DiffuseBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection)) {
		return {};
	}
	Vec3 wi = SampleCosineHemisphere(u);
	if (wo.z < 0) wi.z *= -1;
	Float pdf = CosineHemispherePDF(AbsCosTheta(wi));
	return BSDFSample(m_spectrum * InvPi, wi, pdf, BxDFFlags::DiffuseReflection);
}

Float DiffuseBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection) || !SameHemisphere(wo, wi)) {
		return 0;
	}
	return CosineHemispherePDF(AbsCosTheta(wi));
}

BxDFFlags DiffuseBxDF::Flags() const {
	return m_spectrum ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset;
}

/*
	Diffuse Transmission BxDF
*/

DiffuseTransmissionBxDF::DiffuseTransmissionBxDF(Spectrum r, Spectrum t) : m_r(r), m_t(t) {}

Spectrum DiffuseTransmissionBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	return SameHemisphere(wo, wi) ? (m_r * InvPi) : (m_t * InvPi);
}

std::optional<BSDFSample> DiffuseTransmissionBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Float pr = m_r.MaxComponent(), pt = m_t.MaxComponent();
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
		pr = 0;
	}
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
		pt = 0;
	}
	if (pr == 0 && pt == 0) {
		return {};
	}

	if (uc < pr / (pr + pt)) {
		Vec3 wi = SampleCosineHemisphere(u);
		if (wo.z < 0) {
			wi.z *= -1;
		}
		Float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pr / (pr + pt);
		return BSDFSample(SampleDistribution(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseReflection);
	}
	else {
		Vec3 wi = SampleCosineHemisphere(u);
		if (wo.z > 0) {
			wi.z *= -1;
		}
		Float pdf = CosineHemispherePDF(AbsCosTheta(wi)) * pt / (pr + pt);
		return BSDFSample(SampleDistribution(wo, wi, mode), wi, pdf, BxDFFlags::DiffuseTransmission);
	}
}

Float DiffuseTransmissionBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	Float pr = m_r.MaxComponent(), pt = m_t.MaxComponent();
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
		pr = 0;
	}
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
		pt = 0;
	}
	if (pr == 0 && pt == 0) {
		return {};
	}

	if (SameHemisphere(wo, wi)) {
		return pr / (pr + pt) * CosineHemispherePDF(AbsCosTheta(wi));
	}
	else {
		return pt / (pr + pt) * CosineHemispherePDF(AbsCosTheta(wi));
	}
}

BxDFFlags DiffuseTransmissionBxDF::Flags() const {
	return (BxDFFlags)((m_r ? BxDFFlags::DiffuseReflection : BxDFFlags::Unset) | (m_t ? BxDFFlags::DiffuseTransmission : BxDFFlags::Unset));
}

/*
	Conductor BxDF
*/

ConductorBxDF::ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib, Spectrum eta, Spectrum k) :
	m_mfDistrib(mfDistrib), m_eta(eta), m_k(k) {}

BxDFFlags ConductorBxDF::Flags() const {
	return m_mfDistrib.EffectivelySmooth() ? BxDFFlags::SpecularReflection : BxDFFlags::GlossyReflection;
}

Spectrum ConductorBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (!SameHemisphere(wo, wi) || m_mfDistrib.EffectivelySmooth()) {
		return Spectrum();
	}
	Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
	if (cosTheta_i == 0 || cosTheta_o == 0) {
		return Spectrum();
	}
	Vec3 wm = wi + wo;
	if (length2(wm) == 0) {
		return Spectrum();
	}
	wm = glm::normalize(wm);

	Spectrum F = FrComplex(AbsDot(wo, wm), m_eta, m_k);

	return m_mfDistrib.D(wm) * F * m_mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
}

std::optional<BSDFSample> ConductorBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection)) {
		return {};
	}
	if (m_mfDistrib.EffectivelySmooth()) {
		Vec3 wi(-wo.x, -wo.y, wo.z);
		Spectrum f = FrComplex(AbsCosTheta(wi), m_eta, m_k) / AbsCosTheta(wi);
		return BSDFSample(f, wi, 1.0f, BxDFFlags::SpecularReflection);
	}

	if (wo.z == 0) {
		return {};
	}
	Vec3 wm = m_mfDistrib.Sample_wm(wo, u);
	Vec3 wi = Reflect(wo, wm);
	if (!SameHemisphere(wo, wi)) {
		return {};
	}

	Float pdf = m_mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));

	Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
	if (cosTheta_i == 0 || cosTheta_o == 0) {
		return {};
	}
	Spectrum F = FrComplex(AbsDot(wo, wm), m_eta, m_k);

	Spectrum f = m_mfDistrib.D(wm) * F * m_mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
	return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
}

Float ConductorBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!((uint32_t)sampleFlags & (uint32_t)BxDFReflTransFlags::Reflection) || !SameHemisphere(wo, wi) || m_mfDistrib.EffectivelySmooth()) {
		return 0;
	}
	Vec3 wm = wo + wi;
	if (length2(wm) == 0) {
		return 0;
	}
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));
	return m_mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));
}

void ConductorBxDF::Regularize() {
	m_mfDistrib.Regularize();
}

/*
	Dielectric DxDF
*/

DielectricBxDF::DielectricBxDF(Float refraction, TrowbridgeReitzDistribution mfDistrib) :
	m_refraction(refraction), m_mfDistrib(mfDistrib) {}

BxDFFlags DielectricBxDF::Flags() const {
	BxDFFlags flags = BxDFFlags((m_refraction == 1.0) ? BxDFFlags::Transmission : (BxDFFlags::Reflection | BxDFFlags::Transmission));
	return BxDFFlags(flags | (m_mfDistrib.EffectivelySmooth() ? BxDFFlags::Specular : BxDFFlags::Glossy));
}

std::optional<BSDFSample> DielectricBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (m_refraction == (Float)1.0f || m_mfDistrib.EffectivelySmooth()) {
		Float R = FrDielectric(CosTheta(wo), m_refraction), T = 1 - R;
		Float pr = R, pt = T;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
			pr = 0;
		}
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
			pt = 0;
		}
		if (pr == 0 && pt == 0) {
			return {};
		}

		if (uc < pr / (pr + pt)) {
			Vec3 wi(-wo.x, -wo.y, wo.z);
			Vec3 fr(R / AbsCosTheta(wi));
			return BSDFSample(fr, wi, pr / (pr + pt), BxDFFlags::SpecularReflection);
		}
		else {
			Vec3 wi;
			Float etap;
			bool valid = Refract(wo, Vec3(0, 0, 1), m_refraction, &etap, &wi);
			if (!valid) {
				return {};
			}

			Vec3 ft(T / AbsCosTheta(wi));
			if (mode == TransportMode::Radiance) {
				ft /= Sqr(etap);
			}

			Float pdf = pt / (pr + pt);
			return BSDFSample(ft, wi, pdf, BxDFFlags::SpecularTransmission, etap);
		}
	}
	else {
		Vec3 wm = m_mfDistrib.Sample_wm(wo, u);
		Float R = FrDielectric(glm::dot(wo, wm), m_refraction);
		Float T = 1.0f - R;
		Float pr = R, pt = T;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
			pr = 0;
		}
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
			pt = 0;
		}
		if (pr == 0 && pt == 0) {
			return {};
		}

		Float pdf;
		if (uc < pr / (pr + pt)) {
			Vec3 wi = Reflect(wo, wm);
			if (!SameHemisphere(wo, wi)) {
				return {};
			}
			pdf = m_mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm)) * pr / (pr + pt);

			Vec3 f(m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * R / (4 * CosTheta(wi) * CosTheta(wo)));
			return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
		}
		else {
			Float etap;
			Vec3 wi;
			bool tir = !Refract(wo, wm, m_refraction, &etap, &wi);
			if (SameHemisphere(wo, wi) || wi.z == 0 || tir) {
				return {};
			}

			Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
			Float dwm_dwi = AbsDot(wi, wm) / denom;
			pdf = m_mfDistrib.PDF(wo, wm) * dwm_dwi * pt / (pr + pt);

			Vec3 ft(T * m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / (CosTheta(wi) * CosTheta(wo) * denom)));
			if (mode == TransportMode::Radiance) {
				ft /= Sqr(etap);
			}

			return BSDFSample(ft, wi, pdf, BxDFFlags::GlossyTransmission, etap);
		}
	}
}

Spectrum DielectricBxDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	if (m_refraction == 1.0f || m_mfDistrib.EffectivelySmooth()) {
		return Spectrum();
	}

	Float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	Float etap = 1;
	if (!reflect) {
		etap = cosTheta_o > 0 ? m_refraction : (1 / m_refraction);
	}
	Vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || length2(wm) == 0) {
		return Spectrum();
	}
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0) {
		return Spectrum();
	}

	Float F = FrDielectric(glm::dot(wo, wm), m_refraction);
	if (reflect) {
		return Spectrum(m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * F / std::abs(4 * cosTheta_i * cosTheta_o));
	}
	else {
		Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap) * cosTheta_i * cosTheta_o;
		Float D = m_mfDistrib.D(wm);
		Float G = m_mfDistrib.G(wo, wi);
		Float ft = m_mfDistrib.D(wm) * (1 - F) * m_mfDistrib.G(wo, wi) * std::abs(glm::dot(wi, wm) * glm::dot(wo, wm) / denom);

		if (mode == TransportMode::Radiance) {
			ft /= Sqr(etap);
		}

		return Spectrum(ft);
	}
}

Float DielectricBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth()) {
		return 0;
	}

	Float cosTheta_o = CosTheta(wo), cosTheta_i = CosTheta(wi);
	bool reflect = cosTheta_i * cosTheta_o > 0;
	Float etap = 1;
	if (!reflect) {
		etap = cosTheta_o > 0 ? m_refraction : (1 / m_refraction);
	}
	Vec3 wm = wi * etap + wo;
	if (cosTheta_i == 0 || cosTheta_o == 0 || length2(wm) == 0) {
		return 0;
	}
	wm = FaceForward(glm::normalize(wm), Vec3(0, 0, 1));

	if (glm::dot(wm, wi) * cosTheta_i < 0 || glm::dot(wm, wo) * cosTheta_o < 0) {
		return 0;
	}

	Float R = FrDielectric(glm::dot(wo, wm), m_refraction);
	Float T = 1 - R;

	Float pr = R, pt = T;
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
		pr = 0;
	}
	if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
		pt = 0;
	}
	if (pr == 0 && pt == 0) {
		return 0;
	}

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

/*
	BSDF
*/

BSDF::BSDF(const Material& material, const RayInteraction& intr) :
	m_frame(Frame::FromZ(intr.normal)) {
	if (material.m_transparency > 0.0f) {
		Float alpha = TrowbridgeReitzDistribution::RoughnessToAlpha(material.m_roughness);
		TrowbridgeReitzDistribution distrib(alpha, alpha);
		m_bxdf = new DielectricBxDF(material.m_refraction, distrib);
	}
	else if (material.m_metallic > 0.0f) {
		Float alpha = TrowbridgeReitzDistribution::RoughnessToAlpha(material.m_roughness);
		TrowbridgeReitzDistribution distrib(alpha, alpha);
		m_bxdf = new ConductorBxDF(distrib, Vec3(1.0f), material.m_albedo * TwoPi);
	}
	else {
		m_bxdf = new DiffuseBxDF(material.m_albedo);
	}
}

BSDF::~BSDF() {
	delete m_bxdf;
}

BSDF::operator bool() const {
	return m_bxdf != nullptr;
}

BxDFFlags BSDF::Flags() const {
	return m_bxdf->Flags();
}

Spectrum BSDF::SampleDistribution(Vec3 wo, Vec3 wi, TransportMode mode) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	return m_bxdf->SampleDistribution(wo, wi, mode);
}

std::optional<BSDFSample> BSDF::SampleDirectionAndDistribution(Vec3 wo, Float u, Vec2 u2, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (!(Flags() & (int32_t)sampleFlags)) {
		return {};
	}
	wo = m_frame.ToLocal(wo);
	std::optional<BSDFSample> sample = m_bxdf->SampleDirectionAndDistribution(wo, u, u2, mode, sampleFlags);
	if (sample) {
		sample->wi = m_frame.FromLocal(sample->wi);
	}
	return sample;
}

Float BSDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	wo = m_frame.ToLocal(wo);
	wi = m_frame.ToLocal(wi);
	return m_bxdf->PDF(wo, wi, mode, sampleFlags);
}

Spectrum BSDF::rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u) const {
	wo = m_frame.ToLocal(wo);
	return m_bxdf->rho(wo, uc, u);
}

void BSDF::Regularize() {
	m_bxdf->Regularize();
}
