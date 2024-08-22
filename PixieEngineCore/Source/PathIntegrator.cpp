#include "pch.h"
#include "PathIntegrator.h"

PathIntegrator::PathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler({}) {}

void PathIntegrator::SetScene(Scene* scene) {
	StopRender();
	m_scene = scene;
	m_lightSampler = UniformLightSampler(m_scene->GetGeometrySnapshot()->GetAreaLights());
	Reset();
	StartRender();
}

Spectrum PathIntegrator::Integrate(Ray ray, Sampler* sampler) {
	Spectrum L(0.0f, 0.0f, 0.0f), beta(1.0f, 1.0f, 1.0f);
	int32_t depth = 0;
	Float p_b = 0.0f, etaScale = 1.0f;
	bool specularBounce = false, anyNonSpecularBounces = false;
	SurfaceInteraction prevIntrCtx;

	while (true) {
		m_stats.m_rayCountBuffer.Increment(ray.x, ray.y);
		SurfaceInteraction isect;
		if (!m_scene->Intersect(ray, isect, m_stats)) {
			for (Light* light : m_scene->GetInfiniteLights()) {
				Spectrum Le = light->Le(ray);
				if (depth == 0 || specularBounce) {
					L += beta * Le;
				}
				else {
					Float p_l = m_lightSampler.PMF(prevIntrCtx, light) * light->PDF_Li(prevIntrCtx, ray.direction, true);
					Float w_b = PowerHeuristic(1, p_b, 1, p_l);
					L += beta * w_b * Le;
				}
			}
			break;
		}

		Spectrum Le = isect.Le(-ray.direction);
		if (Le) {
			if (depth == 0 || specularBounce) {
				L += beta * Le;
			}
			else {
				Float p_l = m_lightSampler.PMF(prevIntrCtx, isect.areaLight) * isect.areaLight->PDF_Li(prevIntrCtx, ray.direction, true);
				Float w_l = PowerHeuristic(1, p_b, 1, p_l);
				L += beta * w_l * Le;
			}
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

		if (m_regularize && anyNonSpecularBounces) {
			bsdf.Regularize();
		}

		if (IsNonSpecular(bsdf.Flags())) {
			Spectrum Ld = SampleLd(ray.x, ray.y, isect, bsdf, sampler);
			L += beta * Ld;
		}

		Vec3 wo = -ray.direction;
		BSDFSample bs = bsdf.SampleDirectionAndDistribution(isect.triangle->shadingFrame, wo, sampler->Get(), sampler->Get2D());
		if (!bs) {
			break;
		}

		beta *= bs.f * AbsDot(bs.wi, isect.normal) / bs.pdf;
		p_b = bs.pdfIsProportional ? bsdf.PDF(isect.triangle->shadingFrame, wo, bs.wi) : bs.pdf;
		specularBounce = bs.IsSpecular();
		anyNonSpecularBounces |= !bs.IsSpecular();
		if (bs.IsTransmission()) {
			etaScale *= Sqr(bs.eta);
		}

		Float rrBeta = MaxComponent(beta.GetRGBValue() * etaScale);
		if (rrBeta < 1.0f && depth > 1) {
			Float q = std::max<Float>(0.0f, 1.0f - rrBeta);
			if (RandomFloat() < q) {
				break;
			}
			beta /= 1.0f - q;
		}

		prevIntrCtx = isect;

		ray = isect.SpawnRay(ray, bsdf, bs.wi, bs.flags, bs.eta);
	}

	return L;
}

Spectrum PathIntegrator::SampleLd(uint32_t x, uint32_t y, const SurfaceInteraction& intr, const BSDF& bsdf, Sampler* sampler) {
	std::optional<SampledLight> sampledLight = m_lightSampler.Sample(sampler->Get());
	if (!sampledLight) {
		return Spectrum();
	}

	std::optional<LightLiSample> ls = sampledLight->light->SampleLi(intr, sampler->Get2D());
	if (!ls || !ls->L || ls->pdf <= 0) {
		return Spectrum();
	}

	Vec3 wo = intr.wo, wi = ls->wi;
	Spectrum f = bsdf.SampleDistribution(intr.triangle->shadingFrame, wo, wi) * AbsDot(wi, intr.normal);
	if (!f || !Unoccluded(x, y, intr, ls->pLight)) {
		return Spectrum();
	}

	Float p_l = sampledLight->p * ls->pdf;
	Float p_b = bsdf.PDF(intr.triangle->shadingFrame, wo, wi);
	Float w_l = PowerHeuristic(1, p_l, 1, p_b);
	return w_l * ls->L * f / p_l;
}
