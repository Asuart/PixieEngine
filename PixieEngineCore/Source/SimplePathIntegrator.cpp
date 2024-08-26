#include "pch.h"
#include "SimplePathIntegrator.h"

SimplePathIntegrator::SimplePathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler(new UniformLightSampler()) {}

SimplePathIntegrator::~SimplePathIntegrator() {
	delete m_lightSampler;
}

void SimplePathIntegrator::SetScene(Scene* scene) {
	StopRender();
	m_scene = scene;
	if (m_lightSampler) delete m_lightSampler;
	m_lightSampler = new UniformLightSampler((const std::vector<Light*>&)m_scene->GetGeometrySnapshot()->GetAreaLights());
	Reset();
	StartRender();
}

Spectrum SimplePathIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
	bool specularBounce = true;
	int32_t depth = 0;

	while (beta) {
		RayTracingStatistics::IncrementRays();
		std::optional<ShapeIntersection> si = m_scene->Intersect(ray);
		if (!si) {
			if (!m_sampleLights || specularBounce) {
				for (Light* light : m_scene->GetInfiniteLights()) {
					L += beta * light->Le(ray);
				}
			}
			break;
		}

		SurfaceInteraction intr = si->intr;
		if (!m_sampleLights || specularBounce) {
			L += beta * intr.Le(-ray.direction);
		}

		if (depth++ == m_maxDepth) {
			break;
		}

		BSDF bsdf = intr.GetBSDF(ray, m_scene->GetMainCamera(), sampler);
		if (!bsdf) {
			specularBounce = true;
			intr.SkipIntersection(ray);
			continue;
		}

		Vec3 wo = -ray.direction;
		if (m_sampleLights) {
			std::optional<SampledLight> sampledLight = m_lightSampler->Sample(sampler->Get());
			if (sampledLight) {
				std::optional<LightLiSample> ls = sampledLight->light->SampleLi(intr, sampler->Get2D());
				if (ls && ls->L && ls->pdf > 0) {
					Spectrum f = bsdf.SampleDistribution(wo, ls->wi) * AbsDot(ls->wi, intr.normal);
					if (f && Unoccluded(intr, ls->pLight)) {
						L += beta * f * ls->L / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (m_sampleBSDF) {
			BSDFSample bs = bsdf.SampleDirectionAndDistribution(wo, sampler->Get(), sampler->Get2D());
			if (!bs) {
				break;
			}
			beta *= bs.f * AbsDot(bs.wi, intr.normal) / bs.pdf;
			specularBounce = bs.IsSpecular();
			ray = Ray(intr.position, bs.wi);
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