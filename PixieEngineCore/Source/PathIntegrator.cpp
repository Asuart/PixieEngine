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
	LightSampleContext prevIntrCtx;

	while (beta != glm::fvec3(0.0)) {
		//SurfaceInteraction isect;
		//if (!m_scene->Intersect(ray, isect)) {
		//	for (const auto& light : infiniteLights) {
		//		glm::fvec3 Le = light.Le(ray);
		//		if (depth == 0 || specularBounce) {
		//			L += beta * Le;
		//		}
		//		else {
		//			Float p_l = lightSampler.PMF(prevIntrCtx, light) * light.PDF_Li(prevIntrCtx, ray.direction, true);
		//			Float w_b = PowerHeuristic(1, p_b, 1, p_l);
		//			L += beta * w_b * Le;
		//		}
		//	}
		//	break;
		//}
		//
		//glm::fvec3 Le = si->intr.Le(-ray.direction);
		//if (Le != glm::fvec3(0.0f)) {
		//	if (depth == 0 || specularBounce) {
		//		L += beta * Le;
		//	}
		//	else {
		//		Light areaLight(si->intr.areaLight);
		//		Float lightPDF = lightSampler.PMF(prevIntrCtx, areaLight) * areaLight.PDF_Li(prevIntrCtx, ray.d, true);
		//		Float w_l = PowerHeuristic(1, bsdfPDF, 1, lightPDF);
		//		L += beta * w_l * Le;
		//	}
		//}
		//
		//BSDF bsdf = isect.material->GetBSDF(isect); // isect.material->GetBSDF(ray, camera, sampler);
		//if (!bsdf) {
		//	//isect.SkipIntersection(&ray, si->tHit);
		//	continue;
		//}
		//
		//if (depth == 0 && visibleSurf) {
		//	constexpr int nRhoSamples = 16;
		//	const Float ucRho[nRhoSamples] = { 0.75741637, 0.37870818, 0.7083487, 0.18935409, 0.9149363, 0.35417435, 0.5990858, 0.09467703, 0.8578725, 0.45746812, 0.686759, 0.17708716, 0.9674518, 0.2995429, 0.5083201, 0.047338516 };
		//	const glm::vec2 uRho[nRhoSamples] = { glm::vec2(0.855985, 0.570367),  glm::vec2(0.381823, 0.851844),  glm::vec2(0.285328, 0.764262),  glm::vec2(0.733380, 0.114073),  glm::vec2(0.542663, 0.344465),  glm::vec2(0.127274, 0.414848),  glm::vec2(0.964700, 0.947162),  glm::vec2(0.594089, 0.643463),  glm::vec2(0.095109, 0.170369),  glm::vec2(0.825444, 0.263359),  glm::vec2(0.429467, 0.454469),  glm::vec2(0.244460, 0.816459),  glm::vec2(0.756135, 0.731258),  glm::vec2(0.516165, 0.152852),  glm::vec2(0.180888, 0.214174),  glm::vec2(0.898579, 0.503897) };
		//	glm::fvec3 albedo = bsdf.rho(isect.wo, ucRho, uRho);
		//	*visibleSurf = VisibleSurface(isect, albedo);
		//}
		//
		//if (m_regularize && anyNonSpecularBounces) {
		//	bsdf.Regularize();
		//}
		//
		//if (depth++ == m_maxDepth) {
		//	break;
		//}
		//
		//if (IsNonSpecular(bsdf.Flags())) {
		//	glm::fvec3 Ld = SampleLd(isect, &bsdf);
		//	L += beta * Ld;
		//}
		//
		//Vec3 wo = -ray.d;
		//Float u = RandomFloat();
		//std::optional<BSDFSample> bs = bsdf.Sample_f(wo, u, {RandomFloat(), RandomFloat()});
		//if (!bs) {
		//	break;
		//}
		//
		//beta *= bs->f * glm::abs(glm::dot(bs->wi, isect.n)) / bs->pdf;
		//p_b = bs->pdfIsProportional ? bsdf.PDF(wo, bs->wi) : bs->pdf;
		//specularBounce = bs->IsSpecular();
		//anyNonSpecularBounces |= !bs->IsSpecular();
		//if (bs->IsTransmission())
		//	etaScale *= Sqr(bs->eta);
		//prevIntrCtx = si->intr;
		//
		//ray = isect.SpawnRay(ray, bsdf, bs->wi, bs->flags, bs->eta);
		//
		//glm::fvec3 rrBeta = beta * etaScale;
		//if (MaxComponent(rrBeta) < 1.0f && depth > 1) {
		//	Float q = std::max<Float>(0.0f, 1.0f - MaxComponent(rrBeta));
		//	if (RandomFloat() < q) {
		//		break;
		//	}
		//	beta /= 1.0f - q;
		//}
		break;
	}

	return L;
}

//Vec3 CPURayTracer::SampleLd(const SurfaceInteraction& intr, const BSDF* bsdf) const {
//	LightSampleContext ctx(intr.p, intr.n);
//
//	BxDFFlags flags = bsdf->Flags();
//	if (IsReflective(flags) && !IsTransmissive(flags))
//		ctx.p += intr.wo * ShadowEpsilon;
//	else if (IsTransmissive(flags) && !IsReflective(flags))
//		ctx.p -= intr.wo * ShadowEpsilon;
//
//	Float u = RandomFloat();
//	std::optional<SampledLight> sampledLight = lightSampler->Sample(u);
//	Vec2 uLight = Vec2(RandomFloat(), RandomFloat());
//	if (!sampledLight)
//		return {};
//
//	DiffuseAreaLight light = sampledLight->light;
//	std::optional<LightLiSample> ls = light.SampleLi(intr, uLight);
//	if (!ls || ls->L == glm::fvec3(0) || ls->pdf == 0)
//		return {};
//
//	Vec3 wo = intr.wo, wi = ls->wi;
//	glm::fvec3 f = bsdf->f(wo, wi) * glm::abs(glm::dot(wi, intr.n));
//	if (f == glm::fvec3(0) || !Unoccluded(intr, ls->pLight))
//		return {};
//
//	Float p_l = sampledLight->p * ls->pdf;
//	Float p_b = bsdf->PDF(wo, wi);
//	Float w_l = PowerHeuristic(1, p_l, 1, p_b);
//	return (glm::fvec3)w_l * ls->L * f / (glm::fvec3)p_l;
//}
