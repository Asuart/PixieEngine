#include "pch.h"
#include "ConductorBxDF.h"

ConductorBxDF::ConductorBxDF(const TrowbridgeReitzDistribution& mfDistrib, Spectrum eta, Spectrum k)
	: m_mfDistrib(mfDistrib), m_eta(eta), m_k(k) {}

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

BSDFSample ConductorBxDF::SampleDirectionAndDistribution(Vec3 wo, Float uc, Vec2 u, TransportMode mode, BxDFReflTransFlags sampleFlags) const {
    Vector3f wo, Float uc, Point2f u, TransportMode mode,
        BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
            if (!(sampleFlags & BxDFReflTransFlags::Reflection))
                return {};
            if (mfDistrib.EffectivelySmooth()) {
                // Sample perfect specular conductor BRDF
                Vector3f wi(-wo.x, -wo.y, wo.z);
                SampledSpectrum f = FrComplex(AbsCosTheta(wi), eta, k) / AbsCosTheta(wi);
                return BSDFSample(f, wi, 1, BxDFFlags::SpecularReflection);
            }
            // Sample rough conductor BRDF
            // Sample microfacet normal $\wm$ and reflected direction $\wi$
            if (wo.z == 0)
                return {};
            Vector3f wm = mfDistrib.Sample_wm(wo, u);
            Vector3f wi = Reflect(wo, wm);
            if (!SameHemisphere(wo, wi))
                return {};

            // Compute PDF of _wi_ for microfacet reflection
            Float pdf = mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));

            Float cosTheta_o = AbsCosTheta(wo), cosTheta_i = AbsCosTheta(wi);
            if (cosTheta_i == 0 || cosTheta_o == 0)
                return {};
            // Evaluate Fresnel factor _F_ for conductor BRDF
            SampledSpectrum F = FrComplex(AbsDot(wo, wm), eta, k);

            SampledSpectrum f =
                mfDistrib.D(wm) * F * mfDistrib.G(wo, wi) / (4 * cosTheta_i * cosTheta_o);
            return BSDFSample(f, wi, pdf, BxDFFlags::GlossyReflection);
}

Float ConductorBxDF::PDF(Vec3 wo, Vec3 wi, TransportMode mode, BxDFReflTransFlags sampleFlags = BxDFReflTransFlags::All) const {
    if (!(sampleFlags & BxDFReflTransFlags::Reflection))
        return 0;
    if (!SameHemisphere(wo, wi))
        return 0;
    if (mfDistrib.EffectivelySmooth())
        return 0;
    // Evaluate sampling PDF of rough conductor BRDF
    Vector3f wm = wo + wi;
    CHECK_RARE(1e-5f, LengthSquared(wm) == 0);
    if (LengthSquared(wm) == 0)
        return 0;
    wm = FaceForward(Normalize(wm), Normal3f(0, 0, 1));
    return mfDistrib.PDF(wo, wm) / (4 * AbsDot(wo, wm));
}

void ConductorBxDF::Regularize() { 
    mfDistrib.Regularize(); 
}
