#include "PathIntegrator.h"

PathIntegrator::PathIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution), m_lightSampler({}) {}

void PathIntegrator::SetScene(RTScene* scene) {
	m_scene = scene;
	m_lightSampler = UniformLightSampler(m_scene->lights);
}

Vec3 PathIntegrator::Integrate(Ray ray, Sampler* sampler) {
	glm::fvec3 L(0.0f), beta(1.0f);
	int32_t depth = 0;
	Float p_b = 0.0f, etaScale = 1.0f;
	bool specularBounce = false, anyNonSpecularBounces = false;
	SurfaceInteraction prevIntrCtx;

	//while (beta != glm::fvec3(0.0)) {
	//	SurfaceInteraction isect;
	//	if (!m_scene->Intersect(ray, isect, m_stats)) {
	//		for (const auto& light : infiniteLights) {
	//			glm::fvec3 Le = light.Le(ray);
	//			if (depth == 0 || specularBounce) {
	//				L += beta * Le;
	//			}
	//			else {
	//				Float p_l = lightSampler.PMF(prevIntrCtx, light) * light.PDF_Li(prevIntrCtx, ray.direction, true);
	//				Float w_b = PowerHeuristic(1, p_b, 1, p_l);
	//				L += beta * w_b * Le;
	//			}
	//		}
	//		break;
	//	}
	//	
	//	glm::fvec3 Le = isect.Le(-ray.direction);
	//	if (Le != glm::fvec3(0.0f)) {
	//		if (depth == 0 || specularBounce) {
	//			L += beta * Le;
	//		}
	//		else {				
	//			Float lightPDF = lightSampler.PMF(prevIntrCtx, areaLight) * isect.areaLight->PDF_Li(prevIntrCtx, ray.direction, true);
	//			Float w_l = PowerHeuristic(1, bsdfPDF, 1, lightPDF);
	//			L += beta * w_l * Le;
	//		}
	//	}
	//	
	//	BSDF bsdf = isect.material->GetBSDF(isect); // isect.material->GetBSDF(ray, camera, sampler);
	//	if (!bsdf) {
	//		//isect.SkipIntersection(&ray, si->tHit);
	//		continue;
	//	}
	//	
	//	if (depth == 0 && visibleSurf) {
	//		constexpr int nRhoSamples = 16;
	//		const Float ucRho[nRhoSamples] = { 0.75741637f, 0.37870818f, 0.7083487f, 0.18935409f, 0.9149363f, 0.35417435f, 0.5990858f, 0.09467703f, 0.8578725f, 0.45746812f, 0.686759f, 0.17708716f, 0.9674518f, 0.2995429f, 0.5083201f, 0.047338516f };
	//		const glm::vec2 uRho[nRhoSamples] = { glm::vec2(0.855985f, 0.570367f),  glm::vec2(0.381823f, 0.851844f),  glm::vec2(0.285328f, 0.764262f),  glm::vec2(0.733380f, 0.114073f),  glm::vec2(0.542663f, 0.344465f),  glm::vec2(0.127274f, 0.414848f),  glm::vec2(0.964700f, 0.947162f),  glm::vec2(0.594089f, 0.643463f),  glm::vec2(0.095109f, 0.170369f),  glm::vec2(0.825444f, 0.263359f),  glm::vec2(0.429467f, 0.454469f),  glm::vec2(0.244460f, 0.816459f),  glm::vec2(0.756135f, 0.731258f),  glm::vec2(0.516165f, 0.152852f),  glm::vec2(0.180888f, 0.214174f),  glm::vec2(0.898579f, 0.503897f) };
	//		glm::fvec3 albedo = bsdf.rho(isect.wo, ucRho, uRho);
	//		*visibleSurf = VisibleSurface(isect, albedo);
	//	}
	//	
	//	if (m_regularize && anyNonSpecularBounces) {
	//		bsdf.Regularize();
	//	}
	//	
	//	if (depth++ == m_maxDepth) {
	//		break;
	//	}
	//	
	//	if (IsNonSpecular(bsdf.Flags())) {
	//		glm::fvec3 Ld = SampleLd(isect, &bsdf);
	//		L += beta * Ld;
	//	}
	//	
	//	Vec3 wo = -ray.direction;
	//	Float u = RandomFloat();
	//	std::optional<BSDFSample> bs = bsdf.Sample_f(wo, u, {RandomFloat(), RandomFloat()});
	//	if (!bs) {
	//		break;
	//	}
	//	
	//	beta *= bs->f * glm::abs(glm::dot(bs->wi, isect.normal)) / bs->pdf;
	//	p_b = bs->pdfIsProportional ? bsdf.PDF(wo, bs->wi) : bs->pdf;
	//	specularBounce = bs->IsSpecular();
	//	anyNonSpecularBounces |= !bs->IsSpecular();
	//	if (bs->IsTransmission())
	//		etaScale *= Sqr(bs->eta);
	//	prevIntrCtx = isect;
	//	
	//	ray = isect.SpawnRay(ray, bsdf, bs->wi, bs->flags, bs->eta);
	//	
	//	glm::fvec3 rrBeta = beta * etaScale;
	//	if (MaxComponent(rrBeta) < 1.0f && depth > 1) {
	//		Float q = std::max<Float>(0.0f, 1.0f - MaxComponent(rrBeta));
	//		if (RandomFloat() < q) {
	//			break;
	//		}
	//		beta /= 1.0f - q;
	//	}
	//	break;
	//}

	return L;
}

//Vec3 PathIntegrator::SampleLd(const SurfaceInteraction& intr, const BSDF* bsdf) const {
//	LightSampleContext ctx(intr.position, intr.normal);
//
//	BxDFFlags flags = bsdf->Flags();
//	if (IsReflective(flags) && !IsTransmissive(flags)) {
//		ctx.p += intr.wo * ShadowEpsilon;
//	}
//	else if (IsTransmissive(flags) && !IsReflective(flags)) {
//		ctx.p -= intr.wo * ShadowEpsilon;
//	}
//
//	Float u = RandomFloat();
//	std::optional<SampledLight> sampledLight = lightSampler->Sample(u);
//	Vec2 uLight = Vec2(RandomFloat(), RandomFloat());
//	if (!sampledLight) {
//		return {};
//	}
//
//	DiffuseAreaLight light = sampledLight->light;
//	std::optional<LightLiSample> ls = light.SampleLi(intr, uLight);
//	if (!ls || ls->L == glm::fvec3(0) || ls->pdf == 0) {
//		return {};
//	}
//
//	Vec3 wo = intr.wo, wi = ls->wi;
//	glm::fvec3 f = bsdf->f(wo, wi) * glm::abs(glm::dot(wi, intr.normal));
//	if (f == glm::fvec3(0) || !Unoccluded(intr, ls->pLight)) {
//		return {};
//	}
//
//	Float p_l = sampledLight->p * ls->pdf;
//	Float p_b = bsdf->PDF(wo, wi);
//	Float w_l = PowerHeuristic(1, p_l, 1, p_b);
//	return (glm::fvec3)w_l * ls->L * f / (glm::fvec3)p_l;
//}
