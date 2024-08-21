#include "pch.h"
#include "SimplePathIntegrator.h"

SimplePathIntegrator::SimplePathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler({}) {}

void SimplePathIntegrator::SetScene(Scene* scene) {
	StopRender();
	m_scene = scene;
	m_lightSampler = UniformLightSampler(m_scene->GetGeometrySnapshot()->GetAreaLights());
	Reset();
	StartRender();
}

Spectrum SimplePathIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
	bool specularBounce = true;
	int32_t depth = 0;

	while (beta) {
		SurfaceInteraction isect;
		if (!m_scene->Intersect(ray, isect, m_stats)) {
			if (!m_sampleLights || specularBounce) {
				for (Light* light : m_scene->GetInfiniteLights()) {
					L += beta * light->Le(ray);
				}
			}
			break;
		}

		if (!m_sampleLights || specularBounce) {
			L += beta * isect.Le(-ray.direction);
		}

		if (depth++ == m_maxDepth) {
			break;
		}

		BSDF bsdf = isect.GetBSDF(ray, m_scene->GetMainCamera(), sampler);
		if (!bsdf) {
			specularBounce = true;
			isect.SkipIntersection(ray);
			continue;
		}

		Vec3 wo = -ray.direction;
		if (m_sampleLights) {
			std::optional<SampledLight> sampledLight = m_lightSampler.Sample(sampler->Get());
			if (sampledLight) {
				std::optional<LightLiSample> ls = sampledLight->light->SampleLi(isect, sampler->Get2D());
				if (ls && ls->L && ls->pdf > 0) {
					Spectrum f = bsdf.SampleDistribution(isect.triangle->shadingFrame, wo, ls->wi) * AbsDot(ls->wi, isect.normal);
					if (f && Unoccluded(ray.x, ray.y, isect, ls->pLight)) {
						L += beta * f * ls->L / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (m_sampleBSDF) {
			std::optional<BSDFSample> bs = bsdf.SampleDirectionAndDistribution(isect.triangle->shadingFrame, wo, sampler->Get(), sampler->Get2D());
			if (!bs) {
				break;
			}
			beta *= bs->f * AbsDot(bs->wi, isect.normal) / bs->pdf;
			specularBounce = bs->IsSpecular();
			ray = Ray(ray.x, ray.y, isect.position, bs->wi);
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
				if (IsReflective(flags) && glm::dot(wo, isect.normal) * glm::dot(wi, isect.normal) < 0.0f) {
					wi = -wi;
				}
				else if (IsTransmissive(flags) && glm::dot(wo, isect.normal) * glm::dot(wi, isect.normal) > 0.0f) {
					wi = -wi;
				}
			}
			beta *= bsdf.SampleDistribution(isect.triangle->shadingFrame, wo, wi) * AbsDot(wi, isect.normal) / pdf;
			specularBounce = false;
			ray = Ray(ray.x, ray.y, isect.position, wi);
		}
	}

	return L;
}