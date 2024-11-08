#include "pch.h"
#include "SimplePathIntegrator.h"
#include "SceneSnapshot.h"

SimplePathIntegrator::SimplePathIntegrator()
	: m_lightSampler(new UniformLightSampler({})) {}

SimplePathIntegrator::~SimplePathIntegrator() {
	if (m_lightSampler) delete m_lightSampler;
}

void SimplePathIntegrator::PreprocessSceneSnapshot(SceneSnapshot* sceneSnapshot) {
	if (m_lightSampler) delete m_lightSampler;
	m_lightSampler = new UniformLightSampler((std::vector<Light*>*)&sceneSnapshot->GetAreaLights());
}

Spectrum SimplePathIntegrator::SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) {
	Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
	bool specularBounce = true;
	int32_t depth = 0;

	while (beta) {
		RayTracingStatistics::IncrementRays();
		std::optional<ShapeIntersection> si = RayTracing::Intersect(ray, sceneSnapshot);
		if (!si) {
			if (!m_sampleLights || specularBounce) {
				for (Light* light : sceneSnapshot->GetInfiniteLights()) {
					L += beta * light->Le(ray);
				}
			}
			break;
		}

		SurfaceInteraction intr = si->intr;
		if (!m_sampleLights || specularBounce) {
			L += beta * intr.Le(-ray.direction);
		}

		if (depth++ == RayTracing::c_maxRayBounces) {
			break;
		}

		BSDF bsdf = intr.GetBSDF(ray, sampler);
		if (!bsdf) {
			specularBounce = true;
			intr.SkipIntersection(ray);
			continue;
		}

		Vec3 wo = -ray.direction;
		if (m_sampleLights) {
			std::optional<SampledLight> sampledLight = m_lightSampler->Sample(sampler->Get1D());
			if (sampledLight) {
				std::optional<LightLiSample> ls = sampledLight->light->SampleLi(intr, sampler->Get2D());
				if (ls && ls->L && ls->pdf > 0) {
					Spectrum f = bsdf.SampleDistribution(wo, ls->wi) * AbsDot(ls->wi, intr.normal);
					if (f && RayTracing::Unoccluded(sceneSnapshot, intr, ls->pLight)) {
						L += beta * f * ls->L / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (m_sampleBSDF) {
			std::optional<BSDFSample> bs = bsdf.SampleDirectionAndDistribution(wo, sampler->Get1D(), sampler->Get2D());
			if (!bs) {
				break;
			}
			beta *= bs->f * AbsDot(bs->wi, intr.normal) / bs->pdf;
			specularBounce = bs->IsSpecular();
			ray = Ray(intr.position, bs->wi);
		}
		else {
			Float pdf;
			Vec3 wi;
			BxDFFlags flags = bsdf.Flags();
			if (IsReflective(flags) && IsTransmissive(flags)) {
				wi = SampleUniformSphere(sampler->Get2D());
				pdf = UniformSpherePDF;
			}
			else {
				wi = SampleUniformHemisphere(sampler->Get2D());
				pdf = UniformHemispherePDF;
				if (IsReflective(flags) && glm::dot(wo, intr.normal) * glm::dot(wi, intr.normal) < 0.0f) {
					wi = -wi;
				}
				else if (IsTransmissive(flags) && glm::dot(wo, intr.normal) * glm::dot(wi, intr.normal) > 0.0f) {
					wi = -wi;
				}
			}
			beta *= bsdf.SampleDistribution(wo, wi) * AbsDot(wi, intr.normal) / pdf;
			specularBounce = false;
			ray = Ray(intr.position, wi);
		}
	}

	return L;
}
