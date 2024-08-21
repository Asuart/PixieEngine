#include "pch.h"
#include "DielectricBxDF.h"

DielectricBxDF::DielectricBxDF(Float refraction, TrowbridgeReitzDistribution mfDistrib)
	: m_refraction(refraction), m_mfDistrib(mfDistrib) {}

BxDFFlags DielectricBxDF::Flags() const {
	BxDFFlags flags = BxDFFlags((m_refraction == 1.0) ? BxDFFlags::Transmission : (BxDFFlags::Reflection | BxDFFlags::Transmission));
	return BxDFFlags(flags | (m_mfDistrib.EffectivelySmooth() ? BxDFFlags::Specular : BxDFFlags::Glossy));
}

BSDFSample DielectricBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth()) {
		Float R = FrDielectric(CosTheta(wo), m_refraction), T = 1 - R;
		Float pr = R, pt = T;
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Reflection)) {
			pr = 0;
		}
		if (!((int32_t)sampleFlags & (int32_t)BxDFReflTransFlags::Transmission)) {
			pt = 0;
		}
		if (pr == 0 && pt == 0) {
			return BSDFSample();
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
				return BSDFSample();
			}

			Vec3 ft(T / AbsCosTheta(wi));
			if (mode == TransportMode::Radiance) {
				ft /= Sqr(etap);
			}

			Float pdf = pt / (pr + pt);

			return BSDFSample(Vec3(1), wi, pdf, BxDFFlags::SpecularTransmission, etap);
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
			return BSDFSample();
		}

		Float pdf;
		if (uc < pr / (pr + pt)) {
			Vec3 wi = Reflect(wo, wm);
			if (!SameHemisphere(wo, wi)) {
				return BSDFSample();
			}
			pdf = m_mfDistrib.PDF(wo, wm) / (4 * glm::abs(glm::dot(wo, wm))) * pr / (pr + pt);

			Vec3 f(m_mfDistrib.D(wm) * m_mfDistrib.G(wo, wi) * R / (4 * CosTheta(wi) * CosTheta(wo)));
			return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
		}
		else {
			Float etap;
			Vec3 wi;
			bool tir = !Refract(wo, (Vec3)wm, m_refraction, &etap, &wi);
			if (SameHemisphere(wo, wi) || wi.z == 0 || tir) {
				return BSDFSample();
			}

			Float denom = Sqr(glm::dot(wi, wm) + glm::dot(wo, wm) / etap);
			Float dwm_dwi = glm::abs(glm::dot(wi, wm)) / denom;
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
	if (m_refraction == 1 || m_mfDistrib.EffectivelySmooth()) {
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