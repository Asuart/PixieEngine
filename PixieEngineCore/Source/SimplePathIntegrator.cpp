#include "SimplePathIntegrator.h"

SimplePathIntegrator::SimplePathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler({}) {}

void SimplePathIntegrator::SetScene(RTScene* scene) {
	StopRender();
	m_scene = scene;
	m_lightSampler = UniformLightSampler(m_scene->lights);
	Reset();
	StartRender();
}

Vec3 SimplePathIntegrator::Integrate(Ray ray) {
	glm::fvec3 L(0.0f), beta(1.0f);
	bool specularBounce = true;
	int32_t depth = 0;

	while (beta != glm::fvec3(0.0)) {
		SurfaceInteraction isect;

		if (!m_scene->Intersect(ray, isect, m_stats)) {
			if (!m_sampleLights || specularBounce) {
				L += m_scene->GetSkyColor(ray) * beta;
			}
			break;
		}

		if (!m_sampleLights || specularBounce) {
			L += beta * isect.material->emission; // isect.Le(-ray.d);
		}

		BSDF bsdf = isect.material->GetBSDF(isect); // isect.material->GetBSDF(ray, camera, sampler);
		if (!bsdf) {
			//isect.SkipIntersection(&ray, si->tHit);
			continue;
		}

		if (depth++ == m_maxDepth) {
			break;
		}

		Vec3 wo = -ray.direction;
		if (m_sampleLights) {
			std::optional<SampledLight> sampledLight = m_lightSampler.Sample(RandomFloat());
			if (sampledLight) {
				Vec2 uLight = { RandomFloat(), RandomFloat() };
				std::optional<LightLiSample> ls = sampledLight->light->SampleLi(isect, uLight);
				if (ls && ls->L != glm::fvec3(0.0f) && ls->pdf > 0) {
					Vec3 wi = ls->wi;
					glm::fvec3 f = bsdf.f(wo, wi) * glm::abs(glm::dot(wi, isect.normal));
					if (f != glm::fvec3(0.0f) && Unoccluded(ray.x, ray.y, isect, ls->pLight)) {
						L += beta * f * ls->L / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (m_sampleBSDF) {
			Float u = RandomFloat();
			std::optional<BSDFSample> bs = bsdf.Sample_f(wo, u, {RandomFloat(), RandomFloat()});
			if (!bs) {
				break;
			}
			beta *= bs->f * glm::abs(glm::dot(bs->wi, isect.normal)) / bs->pdf;
			specularBounce = bs->IsSpecular();
			ray = Ray(ray.x, ray.y, isect.position, bs->wi);
		}
		else {
			Float pdf;
			Vec3 wi;
			BxDFFlags flags = bsdf.Flags();
			if (IsReflective(flags) && IsTransmissive(flags)) {
				wi = SampleUniformSphere({ RandomFloat(), RandomFloat() });
				pdf = UniformSpherePDF();
			}
			else {
				wi = SampleUniformHemisphere({ RandomFloat(), RandomFloat() });
				pdf = UniformHemispherePDF();
				if (IsReflective(flags) && glm::dot(wo, isect.normal) * glm::dot(wi, isect.normal) < 0.0f) {
					wi = -wi;
				}
				else if (IsTransmissive(flags) && glm::dot(wo, isect.normal) * glm::dot(wi, isect.normal) > 0.0f) {
					wi = -wi;
				}
			}
			beta *= bsdf.f(wo, wi) * glm::abs(glm::dot(wi, isect.normal)) / pdf;
			specularBounce = false;
			ray = Ray(ray.x, ray.y, isect.position, wi);
		}
	}

	return L;
}