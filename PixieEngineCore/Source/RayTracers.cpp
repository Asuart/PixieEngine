#include "pch.h"
#include "RayTracers.h"
#include "Scene.h"
#include "SceneSnapshot.h"

std::string to_string(RayTracingVisualization mode) {
	switch (mode) {
	case RayTracingVisualization::LightAccumulation: return "Light Accumulation";
	case RayTracingVisualization::BoxTest: return "Box Test";
    case RayTracingVisualization::ShapeTest: return "Shape Test";
    case RayTracingVisualization::Depth: return "Depth";
    case RayTracingVisualization::Normals: return "Normals";
    default: return "Undefined Visualization Mode";
	}
}

std::string to_string(RayTracingMode mode) {
	switch (mode) {
	case RayTracingMode::Naive: return "Naive";
	case RayTracingMode::Volumetric: return "Volumetric";
	default: return "Undefined Ray Tracing Mode";
	}
}

RayTracer* CreateRayTracer(RayTracingMode mode) {
	switch (mode) {
    case RayTracingMode::Volumetric:
        return new VolumetricRayTracer();
	case RayTracingMode::Naive:
	default:
		return new NaiveRayTracer();
	}
}

/*
    Ray Tracer
*/

void RayTracer::SetSceneSnapshot(SceneSnapshot* sceneSnapshot) {
    m_sceneSnapshot = sceneSnapshot;
    Bounds3f sceneBounds = m_sceneSnapshot->GetBounds();
    const std::vector<Light*>& lights = m_sceneSnapshot->GetInfiniteLights();
    for (size_t i = 0; i < lights.size(); i++) {
        lights[i]->Preprocess(sceneBounds);
    }
}

bool RayTracer::Unoccluded(const RayInteraction& p0, const RayInteraction& p1, GBufferPixel& pixel) {
    return Unoccluded(p0.position, p1.position, pixel);
}

bool RayTracer::Unoccluded(Vec3 p0, Vec3 p1, GBufferPixel& pixel) {
    Vec3 dir = p1 - p0;
    Float tMax = glm::length(dir);
    if (tMax < ShadowEpsilon) {
        return false;
    }
    return !m_sceneSnapshot->IsIntersected(Ray(p0, glm::normalize(dir)), &pixel.boxChecks, &pixel.shapeChecks, tMax - ShadowEpsilon);
}

/*
    Naive Ray Tracer
*/

NaiveRayTracer::NaiveRayTracer()
    : m_lightSampler(new UniformLightSampler()) {}

NaiveRayTracer::~NaiveRayTracer() {
    delete m_lightSampler;
}

void NaiveRayTracer::SetSceneSnapshot(SceneSnapshot* sceneSnapshot) {
    m_sceneSnapshot = sceneSnapshot;
    Bounds3f sceneBounds = m_sceneSnapshot->GetBounds();
    const std::vector<Light*>& lights = m_sceneSnapshot->GetInfiniteLights();
    for (size_t i = 0; i < lights.size(); i++) {
        lights[i]->Preprocess(sceneBounds);
    }
    if (m_lightSampler) {
        delete m_lightSampler;
    }
    if (sceneSnapshot) {
        m_lightSampler = new UniformLightSampler(&sceneSnapshot->GetLights());
    }
    else {
        m_lightSampler = new UniformLightSampler({});
    }
}

GBufferPixel NaiveRayTracer::SampleLightRay(Ray ray, Sampler* sampler) {
    GBufferPixel pixel;
    Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
    int32_t depth = 0;
    Float p_b = 0.0f, etaScale = 1.0f;
    bool specularBounce = false, anyNonSpecularBounces = false;
    RayInteraction prevIntrCtx;

    while (true) {
        std::optional<ShapeIntersection> si = m_sceneSnapshot->Intersect(ray, &pixel.boxChecks, &pixel.shapeChecks);
        if (!si) {
            for (Light* light : m_sceneSnapshot->GetInfiniteLights()) {
                Spectrum Le = light->Le(ray);
                if (depth == 0 || specularBounce) {
                    L += beta * Le;
                }
                else {
                    Float p_l = m_lightSampler->PMF(prevIntrCtx, light) * light->SampleLiPDF(prevIntrCtx, ray.direction, true);
                    Float w_b = PowerHeuristic(1, p_b, 1, p_l);
                    L += beta * w_b * Le;
                }
            }
            break;
        }

        RayInteraction intr = si->intr;
        Light* light = intr.lightIndex >= 0 ? m_sceneSnapshot->GetAreaLight(intr.lightIndex) : nullptr;
        Spectrum Le = Spectrum(0.0f);
        if (light) {
            Le = light ? light->L(intr.position, intr.normal, intr.uv, -ray.direction) : Spectrum();
            if (depth == 0 || specularBounce) {
                L += beta * Le;
            }
            else {
                Float p_l = m_lightSampler->PMF(prevIntrCtx, light) * light->SampleLiPDF(prevIntrCtx, ray.direction, true);
                Float w_l = PowerHeuristic(1, p_b, 1, p_l);
                L += beta * w_l * Le;
            }
        }

        if (depth++ == MaxRayBounces) {
            break;
        }

        ProceduralMaterial* material = m_sceneSnapshot->GetMaterial(intr.materialIndex);

        if (IsNonSpecular(material->Flags())) {
            Spectrum Ld = SampleLd(intr, material, sampler, pixel);
            L += beta * Ld;
        }

        Vec3 wo = -ray.direction;
        MaterialSample materialSample = SampleMaterial(material, intr, sampler);

        if (depth == 1) {
            pixel.albedo = materialSample.f;
            pixel.depth = si->tHit;
            pixel.normal = si->intr.normal;
            pixel.position = si->intr.position;
            pixel.uv = si->intr.uv;
        }

        if (!materialSample.f || !materialSample.pdf) {
            break;
        }

        beta *= materialSample.f * AbsDot(materialSample.scattered, intr.normal) / materialSample.pdf;
        p_b = materialSample.pdf;
        specularBounce = IsSpecular(materialSample.flags);
        anyNonSpecularBounces |= !IsSpecular(materialSample.flags);
        if (materialSample.transmission) {
            etaScale *= Sqr(materialSample.refraction);
        }

        Float rrBeta = MaxComponent(beta.GetRGB() * etaScale);
        if (rrBeta < 1.0f && depth > 1) {
            Float q = std::max<Float>(0.0f, 1.0f - rrBeta);
            if (RandomFloat() < q) {
                break;
            }
            beta /= 1.0f - q;
        }

        prevIntrCtx = intr;

        ray = Ray(intr.position, materialSample.scattered);
    }
    pixel.light = L;
    return pixel;
}

MaterialSample NaiveRayTracer::SampleMaterial(ProceduralMaterial* material, RayInteraction& inter, Vec3 wo, Vec3 wi) {
    Frame frame = Frame::FromZ(inter.normal);
    Vec3 woLocal = frame.ToLocal(wo);
    Vec3 wiLocal = frame.ToLocal(wi);
    if (!SameHemisphere(woLocal, wiLocal)) {
        return { Spectrum(0), wi, 0 };
    }
    Spectrum f = material->m_albedo->Sample(inter.uv) * InvPi;
    Float pdf = CosineHemispherePDF(AbsCosTheta(wiLocal));
    return { f, wi, pdf };
}

static Float Reflectance(Float cosine, Float refraction_index) {
    Float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * glm::pow((1.0f - cosine), 5.0f);
}

MaterialSample NaiveRayTracer::SampleMaterial(ProceduralMaterial* material, RayInteraction& inter, Sampler* sampler) {
    Frame frame = Frame::FromZ(inter.normal);
    Vec3 wo = frame.ToLocal(inter.wo);
    Float transperencyTest = RandomFloat();
    Float roughness = material->m_roughness->Sample(inter.uv);
    if (transperencyTest < material->m_transparency) {
        // Transmission
        Spectrum attenuation = material->m_albedo->Sample(inter.uv);
        Float ri = glm::dot(inter.normal, inter.wo) > 0.0f ? (1.0f / material->m_refraction) : material->m_refraction;
        Float cos_theta = CosTheta(wo);
        Float sin_theta = SinTheta(wo);
        bool cannot_refract = ri * sin_theta > 1.0;
        Vec3 wi;
        BxDFFlags flags = BxDFFlags::Unset;
        if (cannot_refract || Reflectance(cos_theta, ri) > sampler->Get1D()) {
            wi = Vec3(-wo.x, -wo.y, wo.z);
            flags = BxDFFlags::SpecularReflection;
        }
        else {
            wi = glm::refract(-wo, Vec3(0.0f, 0.0f, 1.0f), (Float)(glm::dot(inter.normal, inter.wo) > 0.0f ? (1.0f / material->m_refraction) : material->m_refraction));
            flags = BxDFFlags::SpecularTransmission;
        }
        if (wi == Vec3(0.0f)) {
            return { Spectrum(0.0f), frame.FromLocal(wi), 0 };
        }
        return { Spectrum(1.0f), frame.FromLocal(wi), material->m_transparency, flags };
    }
    else {
        Vec3 wi = Vec3(-wo.x, -wo.y, wo.z);
        Float theta = glm::acos(wi.z);
        Float phi = glm::sign(wi.y) * glm::acos(wi.x / (glm::length(Vec2(wi.x, wi.y))));
        Vec2 offset = Pi * SampleUniformDiskConcentric(sampler->Get2D()) * roughness;
        theta += offset.x;
        phi += offset.y;
        Vec3 scattered = Vec3(glm::sin(theta) * glm::cos(phi), glm::sin(theta) * glm::sin(phi), glm::cos(theta));
        if (scattered.z < 0) {
            scattered.z = -scattered.z;
        }
        Spectrum f = material->m_albedo->Sample(inter.uv) / (1 + Pi * roughness);
        return { f, frame.FromLocal(scattered), (1.0f - material->m_transparency) * 1.0f / (1.0f + roughness * Pi), BxDFFlags::DiffuseReflection};
    }
}

Spectrum NaiveRayTracer::SampleLd(RayInteraction& intr, ProceduralMaterial* material, Sampler* sampler, GBufferPixel& pixel) {
    std::optional<SampledLight> sampledLight = m_lightSampler->Sample(sampler->Get1D());
    if (!sampledLight) {
        return Spectrum();
    }

    std::optional<LightLiSample> ls = sampledLight->light->SampleLi(intr, sampler->Get2D());
    if (!ls || !ls->L || ls->pdf <= 0) {
        return Spectrum();
    }

    Vec3 wo = intr.wo, wi = ls->wi;
    MaterialSample materialSample = SampleMaterial(material, intr, wo, wi);
    Spectrum f = materialSample.f * AbsDot(wi, intr.normal);
    if (!f || !Unoccluded(intr, ls->pLight, pixel)) {
        return Spectrum();
    }

    Float p_l = sampledLight->p * ls->pdf;
    Float p_b = materialSample.pdf;
    Float w_l = PowerHeuristic(1, p_l, 1, p_b);
    return w_l * ls->L * f / p_l;
}

/*
	Volumetric Ray Tracer
*/

struct RayMajorantSegment {
    Float tMin, tMax;
    Spectrum sigma_maj;
};

Spectrum ClampZero(Spectrum s) {
    return Spectrum(glm::max(s.GetRGB(), Vec3(0.0f)));
}
      
//template <typename F>
//Spectrum SampleT_maj(Ray ray, Float tMax, Float u, RNG& rng, F callback`) {
//    auto sample = [&](auto medium) {
//        using M = typename std::remove_reference_t<decltype(*medium)>; 
//        return SampleT_maj<M>(ray, tMax, u, rng, callback);
//        };
//    //return ray.medium.Dispatch(sample);
//    return Spectrum(0.0f);
//}
//
//template <typename ConcreteMedium, typename F>
//Spectrum SampleT_maj(Ray ray, Float tMax, Float u, RNG& rng, F callback) {
//    tMax *= Length(ray.d);
//    ray.d = Normalize(ray.d);
//
//    ConcreteMedium* medium = ray.medium.Cast<ConcreteMedium>();
//    typename ConcreteMedium::MajorantIterator iter = medium->SampleRay(ray, tMax);
//
//    Spectrum T_maj(1.0f);
//    bool done = false;
//    while (!done) {
//        std::optional<RayMajorantSegment> seg = iter.Next();
//        if (!seg) {
//            return T_maj;
//        }
//        if (seg->sigma_maj[0] == 0) {
//            Float dt = seg->tMax - seg->tMin;
//            if (IsInf(dt)) {
//                dt = std::numeric_limits<Float>::max();
//            }
//            T_maj *= FastExp(-dt * seg->sigma_maj);
//            continue;
//        }
//
//        Float tMin = seg->tMin;
//        while (true) {
//            Float t = tMin + SampleExponential(u, seg->sigma_maj[0]);
//            u = rng.Uniform<Float>();
//            if (t < seg->tMax) {
//                T_maj *= FastExp(-(t - tMin) * seg->sigma_maj);
//                MediumProperties mp = medium->SamplePoint(ray(t));
//                if (!callback(ray(t), mp, seg->sigma_maj, T_maj)) {
//                    done = true;
//                    break;
//                }
//                T_maj = Spectrum(1.0f);
//                tMin = t;
//            }
//            else {
//                Float dt = seg->tMax - tMin;
//                if (IsInf(dt)) {
//                    dt = std::numeric_limits<Float>::max();
//                }
//                T_maj *= FastExp(-dt * seg->sigma_maj);
//                break;
//            }
//        }
//    }
//    return Spectrum(1.0f);
//}

class Medium {
    float dencity = 0;
};

void VolumetricRayTracer::SetSceneSnapshot(SceneSnapshot* sceneSnapshot) {
    m_sceneSnapshot = sceneSnapshot;
    if (m_lightSampler) delete m_lightSampler;
    if (sceneSnapshot) {
        m_lightSampler = new UniformLightSampler((std::vector<Light*>*) & sceneSnapshot->GetLights());
    }
    else {
        m_lightSampler = new UniformLightSampler({});
    }
}

GBufferPixel VolumetricRayTracer::SampleLightRay(Ray ray, Sampler* sampler) {
    GBufferPixel pixel;
    Spectrum L(0.0f), beta(1.0f), r_u(1.0f), r_l(1.0f);
    bool specularBounce = false, anyNonSpecularBounces = false;
    int32_t depth = 0;
    Float etaScale = 1;
    
    LightSampleContext prevIntrContext;
    
    Medium* currentMedium = nullptr;

    while (true) {
        std::optional<ShapeIntersection> si = m_sceneSnapshot->Intersect(ray, &pixel.boxChecks, &pixel.shapeChecks);
        //if (currentMedium) {
        //    bool scattered = false, terminated = false;
        //    Float tMax = si ? si->tHit : Infinity;
        //    uint64_t hash0 = Hash(sampler->Get1D());
        //    uint64_t hash1 = Hash(sampler->Get1D());
        //    RNG rng(hash0, hash1);
        //
        //    Spectrum T_maj = SampleT_maj(ray, tMax, sampler->Get1D(), rng,
        //        [&](Vec3 p, MediumProperties mp, Spectrum sigma_maj, Spectrum T_maj) {
        //                if (!beta) {
        //                    terminated = true;
        //                    return false;
        //                }
        //                if (depth < MaxRayBounces && mp.Le) {
        //                    Float pdf = sigma_maj.Average() * T_maj.Average();
        //                    Spectrum betap = beta * T_maj / pdf;
        //                    Spectrum r_e = r_u * sigma_maj * T_maj / pdf;
        //                    if (r_e) {
        //                        L += betap * mp.sigma_a * mp.Le / r_e.Average();
        //                    }
        //                }
        //
        //                Float pAbsorb = mp.sigma_a.Average() / sigma_maj.Average();
        //                Float pScatter = mp.sigma_s.Average() / sigma_maj.Average();
        //                Float pNull = std::max<Float>(0, 1 - pAbsorb - pScatter);
        //
        //                Float um = rng.Uniform<Float>();
        //                std::vector<Float> weights = { pAbsorb, pScatter, pNull };
        //                int32_t mode = SampleDiscrete(weights, um);
        //                if (mode == 0) {
        //                    terminated = true;
        //                    return false;
        //                }
        //                else if (mode == 1) {
        //                    if (depth++ >= MaxRayBounces) {
        //                        terminated = true;
        //                        return false;
        //                    }
        //
        //                    Float pdf = T_maj.Average() * mp.sigma_s.Average();
        //                    beta *= T_maj * mp.sigma_s / pdf;
        //                    r_u *= T_maj * mp.sigma_s / pdf;
        //
        //                    if (beta && r_u) {
        //                        RayInteraction intr(p, -ray.direction, ray.medium, mp.phase);
        //                        L += SampleLd(intr, nullptr, sampler, beta, r_u);
        //
        //                        Vec2 u = sampler->Get2D();
        //                        std::optional<PhaseFunctionSample> ps = intr.phase->Sample_p(-ray.direction, u);
        //                        if (!ps || ps->pdf == 0) {
        //                            terminated = true;
        //                        }
        //                        else {
        //                            beta *= ps->p / ps->pdf;
        //                            r_l = r_u / ps->pdf;
        //                            prevIntrContext = LightSampleContext(intr);
        //                            scattered = true;
        //                            ray.origin = p;
        //                            ray.direction = ps->wi;
        //                            specularBounce = false;
        //                            anyNonSpecularBounces = true;
        //                        }
        //                    }
        //                    return false;
        //                }
        //                else {
        //                    Spectrum sigma_n = ClampZero(sigma_maj - mp.sigma_a - mp.sigma_s);
        //                    Float pdf = T_maj.Average() * sigma_n.Average();
        //                    beta *= T_maj * sigma_n / pdf;
        //                    if (pdf == 0) {
        //                        beta = Spectrum(0.0f);
        //                    }
        //                    r_u *= T_maj * sigma_n / pdf;
        //                    r_l *= T_maj * sigma_maj / pdf;
        //                    return beta && r_u;
        //                }
        //        });
        //
        //    if (terminated || !beta || !r_u) {
        //        return L;
        //    }
        //
        //    if (scattered) {
        //        continue;
        //    }
        //
        //    beta *= T_maj / T_maj.Average();
        //    r_u *= T_maj / T_maj.Average();
        //    r_l *= T_maj / T_maj.Average();
        //}
    
        if (!si) {
            for (const Light* light : m_sceneSnapshot->GetInfiniteLights()) {
                if (Spectrum Le = light->Le(ray); Le) {
                    if (depth == 0 || specularBounce) {
                        L += beta * Le / r_u.Average();
                    }
                    else {
                        Float p_l = m_lightSampler->PMF(prevIntrContext, light) * light->SampleLiPDF(prevIntrContext, ray.direction, true);
                        r_l *= p_l;
                        L += beta * Le / (r_u + r_l).Average();
                    }
                }
            }
            break;
        }
    
        RayInteraction& isect = si->intr;
        if (isect.lightIndex >= 0) {
            Light* areaLight = m_sceneSnapshot->GetAreaLight(isect.lightIndex);
            Spectrum Le = areaLight->Le(ray);
            if (depth == 0 || specularBounce) {
                L += beta * Le / r_u.Average();
            }
            else {
                Float p_l = m_lightSampler->PMF(prevIntrContext, areaLight) * areaLight->SampleLiPDF(prevIntrContext, ray.direction, true);
                r_l *= p_l;
                L += beta * Le / (r_u + r_l).Average();
            }
        }
    
        BSDF bsdf = ResourceManager::GetMaterial(isect.materialIndex)->GetBSDF(isect);

        if (!bsdf) {
            ray.SkipIntersection(isect.position);
            continue;
        }
    
        if (depth++ >= MaxRayBounces) {
            break;
        }
    
        if (m_regularize && anyNonSpecularBounces) {
            bsdf.Regularize();
        }
    
        if (IsNonSpecular(bsdf.Flags())) {
            L += SampleLd(isect, &bsdf, sampler, beta, r_u, pixel);
        }
        prevIntrContext = LightSampleContext(isect);
    
        Vec3 wo = isect.wo;
        Float u = sampler->Get1D();
        std::optional<BSDFSample> bs = bsdf.SampleDirectionAndDistribution(wo, u, sampler->Get2D());
        if (!bs) {
            break;
        }
        if (depth == 1) {
            pixel.albedo = bs->f;
            pixel.depth = si->tHit;
            pixel.normal = si->intr.normal;
            pixel.position = si->intr.position;
            pixel.uv = si->intr.uv;
        }
        beta *= bs->f * AbsDot(bs->wi, isect.normal) / bs->pdf;
        if (bs->pdfIsProportional) {
            r_l = r_u / bsdf.PDF(wo, bs->wi);
        }
        else {
            r_l = r_u / bs->pdf;
        }
    
        specularBounce = bs->IsSpecular();
        anyNonSpecularBounces |= !bs->IsSpecular();
        if (bs->IsTransmission()) {
            etaScale *= Sqr(bs->eta);
        }
    
        ray = Ray(isect.position, bs->wi);

        //BSSRDF bssrdf = isect.GetBSSRDF(ray);
        //if (bssrdf && bs->IsTransmission()) {
        //    Float uc = sampler->Get1D();
        //    Vec2 up = sampler->Get2D();
        //    std::optional<BSSRDFProbeSegment> probeSeg = bssrdf.SampleSp(uc, up);
        //    if (!probeSeg) {
        //        break;
        //    }
        //
        //    uint64_t seed = MixBits(FloatToBits(sampler->Get1D()));
        //    WeightedReservoirSampler<SubsurfaceInteraction> interactionSampler(seed);
        //    RayInteraction base(probeSeg->p0, Medium());
        //    while (true) {
        //        Ray r = base.SpawnRayTo(probeSeg->p1);
        //        if (r.direction == Vec3(0, 0, 0)) {
        //            break;
        //        }
        //        std::optional<ShapeIntersection> si = RayTracing::Intersect(r, sceneSnapshot, 1);
        //        if (!si) {
        //            break;
        //        }
        //        base = si->intr;
        //        if (si->intr.material == isect.material) {
        //            interactionSampler.Add(SubsurfaceInteraction(si->intr), 1.f);
        //        }
        //    }
        //
        //    if (!interactionSampler.HasSample()) {
        //        break;
        //    }
        //
        //    SubsurfaceInteraction ssi = interactionSampler.GetSample();
        //    BSSRDFSample bssrdfSample = bssrdf.ProbeIntersectionToSample(ssi);
        //    if (!bssrdfSample.Sp || !bssrdfSample.pdf) {
        //        break;
        //    }
        //
        //    Float pdf = interactionSampler.SampleProbability() * bssrdfSample.pdf[0];
        //    beta *= bssrdfSample.Sp / pdf;
        //    r_u *= bssrdfSample.pdf / bssrdfSample.pdf[0];
        //    RayInteraction pi = ssi;
        //    pi.wo = bssrdfSample.wo;
        //    prevIntrContext = LightSampleContext(pi);
        //    BSDF& Sw = bssrdfSample.Sw;
        //    anyNonSpecularBounces = true;
        //    if (m_regularize) {
        //        Sw.Regularize();
        //    }
        //
        //    L += SampleLd(pi, &Sw, sampler, beta, r_u);
        //
        //    Float u = sampler->Get1D();
        //    std::optional<BSDFSample> bs = Sw.SampleDirectionAndDistribution(pi.wo, u, sampler->Get2D());
        //    if (!bs) {
        //        break;
        //    }
        //    beta *= bs->f * AbsDot(bs->wi, pi.normal) / bs->pdf;
        //    r_l = r_u / bs->pdf;
        //    specularBounce = bs->IsSpecular();
        //    ray = pi.SpawnRay(bs->wi);
        //}
    
        if (!beta) {
            break;
        }
        Spectrum rrBeta = beta * etaScale / r_u.Average();
        Float uRR = sampler->Get1D();
        if (MaxComponent(rrBeta.GetRGB()) < 1.0f && depth > 1) {
            Float q = std::max<Float>(0, 1 - MaxComponent(rrBeta.GetRGB()));
            if (uRR < q) {
                break;
            }
            beta /= 1.0f - q;
        }
    }
    pixel.light = L;
    return pixel;
}

Spectrum VolumetricRayTracer::SampleLd(const RayInteraction& intr, const BSDF* bsdf, Sampler* sampler, Spectrum beta, Spectrum r_p, GBufferPixel& pixel) {
    LightSampleContext ctx;
    if (bsdf) {
        ctx = LightSampleContext(intr);
        BxDFFlags flags = bsdf->Flags();
        if (IsReflective(flags) && !IsTransmissive(flags)) {
            ctx.position = intr.position + intr.wo * ShadowEpsilon;
        }
        else if (IsTransmissive(flags) && !IsReflective(flags)) {
            ctx.position = intr.position - intr.wo * ShadowEpsilon;
        }
    }
    else {
        ctx = LightSampleContext(intr);
    }
    
    Float u = sampler->Get1D();
    std::optional<SampledLight> sampledLight = m_lightSampler->Sample(ctx, u);
    Vec2 uLight = sampler->Get2D();
    if (!sampledLight) {
        return Spectrum(0.0f);
    }
    Light* light = sampledLight->light;
    
    std::optional<LightLiSample> ls = light->SampleLi(ctx, uLight, true);
    if (!ls || !ls->L || ls->pdf == 0) {
        return Spectrum(0.0f);
    }
    Float p_l = sampledLight->p * ls->pdf;
    
    Float scatterPDF;
    Spectrum f_hat;
    Vec3 wo = intr.wo, wi = ls->wi;
    if (bsdf) {
        f_hat = bsdf->SampleDistribution(wo, wi) * AbsDot(wi, intr.normal);
        scatterPDF = bsdf->PDF(wo, wi);
    }
    else {
        //PhaseFunction* phase = intr.phase;
        //f_hat = Spectrum(phase->p(wo, wi));
        //scatterPDF = phase->PDF(wo, wi);
    }
    if (!f_hat) {
        return Spectrum(0.0f);
    }
    
    Ray lightRay = Ray(intr.position, glm::normalize(ls->pLight.position - intr.position));
    Spectrum T_ray(1.f), r_l(1.f), r_u(1.f);
    RNG rng(Hash(lightRay.origin), Hash(lightRay.direction));
    
    while (lightRay.direction != Vec3(0, 0, 0)) {
        std::optional<ShapeIntersection> si = m_sceneSnapshot->Intersect(lightRay, &pixel.boxChecks, &pixel.shapeChecks, 1.0f - ShadowEpsilon);
        if (si) {
            return Spectrum(0.0f);
        }
    
        //if (lightRay.medium) {
        //    Float tMax = si ? si->tHit : (1 - ShadowEpsilon);
        //    Float u = rng.Uniform<Float>();
        //    Spectrum T_maj =
        //        SampleT_maj(lightRay, tMax, u, rng,
        //            [&](Vec3 p, MediumProperties mp, Spectrum sigma_maj, Spectrum T_maj) {
        //                    Spectrum sigma_n = ClampZero(sigma_maj - mp.sigma_a - mp.sigma_s);
        //                    Float pdf = T_maj.Average() * sigma_maj.Average();
        //                    T_ray *= T_maj * sigma_n / pdf;
        //                    r_l *= T_maj * sigma_maj / pdf;
        //                    r_u *= T_maj * sigma_n / pdf;
        //
        //                    Spectrum Tr = T_ray / (r_l + r_u).Average();
        //                    if (MaxComponent(Tr.GetRGB()) < 0.05f) {
        //                        Float q = 0.75f;
        //                        if (rng.Uniform<Float>() < q) {
        //                            T_ray = Spectrum(0.0f);
        //                        }
        //                        else {
        //                            T_ray /= 1 - q;
        //                        }
        //                    }
        //
        //                    if (!T_ray) {
        //                        return false;
        //                    }
        //                    return true;
        //            });
        //    T_ray *= T_maj / T_maj.Average();
        //    r_l *= T_maj / T_maj.Average();
        //    r_u *= T_maj / T_maj.Average();
        //}
    
        if (!T_ray) {
            return Spectrum(0.0f);
        }
        if (!si) {
            break;
        }
        lightRay = Ray(si->intr.position, glm::normalize(ls->pLight.position - si->intr.position));
    }
    
    r_l *= r_p * p_l;
    r_u *= r_p * scatterPDF;
    if (IsDeltaLight(light->Type())) {
        return beta * f_hat * T_ray * ls->L / r_l.Average();
    }
    else {
        return beta * f_hat * T_ray * ls->L / (r_l + r_u).Average();
    }

    return Spectrum(0.0f);
}
