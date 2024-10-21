#include "pch.h"
#include "PathIntegrator.h"

PathIntegrator::PathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler(new UniformLightSampler()){}

PathIntegrator::~PathIntegrator() {
	delete m_lightSampler;
}

void PathIntegrator::SetScene(Scene* scene) {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}
	m_scene = scene;
	if (m_lightSampler) delete m_lightSampler;
	m_lightSampler = new UniformLightSampler((const std::vector<Light*>&)m_scene->GetGeometrySnapshot()->GetAreaLights());
	Reset();
	if (wasRendering) {
		StartRender();
	}
}

Spectrum PathIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
	int32_t depth = 0;
	Float p_b = 0.0f, etaScale = 1.0f;
	bool specularBounce = false, anyNonSpecularBounces = false;
	SurfaceInteraction prevIntrCtx;

	while (true) {
		RayTracingStatistics::IncrementRays();
		std::optional<ShapeIntersection> si = m_scene->Intersect(ray);
		if (!si) {
			for (Light* light : m_scene->GetInfiniteLights()) {
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

		SurfaceInteraction intr = si->intr;
		Spectrum Le = intr.Le(-ray.direction);
		if (Le) {
			if (depth == 0 || specularBounce) {
				L += beta * Le;
			}
			else {
				Float p_l = m_lightSampler->PMF(prevIntrCtx, intr.areaLight) * intr.areaLight->SampleLiPDF(prevIntrCtx, ray.direction, true);
				Float w_l = PowerHeuristic(1, p_b, 1, p_l);
				L += beta * w_l * Le;
			}
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

		if (m_regularize && anyNonSpecularBounces) {
			bsdf.Regularize();
		}

		if (IsNonSpecular(bsdf.Flags())) {
			Spectrum Ld = SampleLd(intr, bsdf, sampler);
			L += beta * Ld;
		}

		Vec3 wo = -ray.direction;
		std::optional<BSDFSample> bs = bsdf.SampleDirectionAndDistribution(wo, sampler->Get1D(), sampler->Get2D());
		if (!bs) {
			break;
		}

		beta *= bs->f * AbsDot(bs->wi, intr.normal) / bs->pdf;
		p_b = bs->pdfIsProportional ? bsdf.PDF(wo, bs->wi) : bs->pdf;
		specularBounce = bs->IsSpecular();
		anyNonSpecularBounces |= !bs->IsSpecular();
		if (bs->IsTransmission()) {
			etaScale *= Sqr(bs->eta);
		}

		Float rrBeta = MaxComponent(beta.GetRGBValue() * etaScale);
		if (rrBeta < 1.0f && depth > 1) {
			Float q = std::max<Float>(0.0f, 1.0f - rrBeta);
			if (RandomFloat() < q) {
				break;
			}
			beta /= 1.0f - q;
		}

		prevIntrCtx = intr;

		ray = intr.SpawnRay(ray, bsdf, bs->wi, bs->flags, bs->eta);
	}

	return L;
}

Spectrum PathIntegrator::SampleLd(const SurfaceInteraction& intr, const BSDF& bsdf, Sampler* sampler) {
	std::optional<SampledLight> sampledLight = m_lightSampler->Sample(sampler->Get1D());
	if (!sampledLight) {
		return Spectrum();
	}

	std::optional<LightLiSample> ls = sampledLight->light->SampleLi(intr, sampler->Get2D());
	if (!ls || !ls->L || ls->pdf <= 0) {
		return Spectrum();
	}

	Vec3 wo = intr.wo, wi = ls->wi;
	Spectrum f = bsdf.SampleDistribution(wo, wi) * AbsDot(wi, intr.normal);
	if (!f || !Unoccluded(intr, ls->pLight)) {
		return Spectrum();
	}

	Float p_l = sampledLight->p * ls->pdf;
	Float p_b = bsdf.PDF(wo, wi);
	Float w_l = PowerHeuristic(1, p_l, 1, p_b);
	return w_l * ls->L * f / p_l;
}
