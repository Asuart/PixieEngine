
#include "CPURayTracer.h"

void clear(std::queue<int>& q) {
	std::queue<int> empty;
	std::swap(q, empty);
}

std::string to_string(CPURayTracerMode mode) {
	switch (mode) {
	case CPURayTracerMode::Normals:
		return "Normals";
	case CPURayTracerMode::TraceRay:
		return "TraceRay";
	case CPURayTracerMode::TracePath:
		return "TracePath";
	case CPURayTracerMode::LiRandomWalk:
		return "LiRandomWalk";
	case CPURayTracerMode::LiSimplePath:
		return "LiSimplePath";
	case CPURayTracerMode::LiPath:
		return "LiPath";
	}
	return "Undefined Ray Tarcer Mode";
}

CPURayTracer::CPURayTracer(glm::ivec2 resolution, int32_t _maxDepth)
	: resolution(resolution), film(resolution), maxDepth(_maxDepth) {
	GenerateQuads();
}

CPURayTracer::~CPURayTracer() {
	if (isRendering) EndRender();
}

void CPURayTracer::GenerateQuads() {
	quads.clear();
	glm::ivec2 tileGridSize = glm::ceil(Vec2(resolution) / Vec2(tileSize));
	for (int32_t yTile = 0; yTile < tileGridSize.y; yTile++) {
		for (int32_t xTile = 0; xTile < tileGridSize.x; xTile++) {
			int32_t xMax = glm::min(resolution.x, (xTile + 1) * tileSize.x);
			int32_t yMax = glm::min(resolution.y, (yTile + 1) * tileSize.y);
			glm::ivec2 min = glm::ivec2(xTile * tileSize.x, yTile * tileSize.y);
			glm::ivec2 max = glm::ivec2(xMax, yMax);
			quads.push_back(Bounds2i(min, max));
		}
	}
}

void CPURayTracer::Resize(glm::ivec2 _resolution) {
	EndRender();
	resolution = _resolution;
	film.Resize(resolution);
	GenerateQuads();
	StartRender();
}

void CPURayTracer::Reset() {
	EndRender();
	film.Reset();
	StartRender();
}

void CPURayTracer::StartRender() {
	if (isRendering) EndRender();
	isRendering = true;
	sample = 0;
	//startTime = Time::GetTime();
	//sampleStartTime = startTime;

	clear(quadQueue);
	for (int32_t i = 0; i < quads.size(); i++) {
		quadQueue.push(i);
	}

	threads = std::min(maxThreads, (int32_t)quads.size());

	for (int32_t i = 0; i < maxThreads && i < quads.size(); i++) {
		renderThreads.push_back(new std::thread(std::thread([&]() {
			while (isRendering) {
				quadQueueMutex.lock();
				if (quadQueue.empty()) {
					//std::chrono::microseconds currrentTime = Time::GetTime();
					//renderTime = (currrentTime - startTime).count() / 1000.0;
					//lastSampleTime = (currrentTime - sampleStartTime).count() / 1000.0;
					//sampleStartTime = currrentTime;
					sample++;
					for (int32_t i = 0; i < quads.size(); i++) {
						quadQueue.push(i);
					}
				}
				int32_t index = quadQueue.front();
				quadQueue.pop();
				quadQueueMutex.unlock();
				Bounds2i quad = quads[index];
				for (int32_t y = quad.pMin.y; y < quad.pMax.y; y++) {
					for (int32_t x = quad.pMin.x; x < quad.pMax.x; x++) {
						PerPixel(x, y);
					}
				}
			}
			})));
	}
}

void CPURayTracer::EndRender() {
	isRendering = false;
	for (int32_t i = 0; i < renderThreads.size(); i++) {
		renderThreads[i]->join();
	}
	for (int32_t i = 0; i < renderThreads.size(); i++) {
		delete renderThreads[i];
	}
	renderThreads.clear();
}

void CPURayTracer::SetScene(RTScene* s) {
	if (isRendering) EndRender();
	scene = s;
	film.Reset();
	if (lightSampler) delete lightSampler;
	lightSampler = new UniformLightSampler(scene->lights);
}

bool CPURayTracer::Unoccluded(const RTInteraction& p0, const RTInteraction& p1) const {
	Vec3 dir = p1.p - p0.p;
	Float tMax = glm::length(dir);
	return !scene->IntersectP(Ray(p0.p, glm::normalize(dir)), tMax - ShadowEpsilon);
}

void CPURayTracer::PerPixel(uint32_t x, uint32_t y) {
	Vec2 uv = film.GetUV(x, y, Vec2(RandomFloat(), RandomFloat()));
	Ray ray = scene->mainCamera->GetRay(uv);
	Vec3 color;
	switch (mode) {
	case CPURayTracerMode::Normals:
		color = TraceNormals(ray);
		break;
	case CPURayTracerMode::TraceRay:
		color = TraceRay(ray);
		break;
	case CPURayTracerMode::TracePath:
		color = TracePath(ray);
		break;
	case CPURayTracerMode::LiRandomWalk:
		color = LiRandomWalk(ray);
		break;
	case CPURayTracerMode::LiSimplePath:
		color = LiSimplePath(ray);
		break;
	case CPURayTracerMode::LiPath:
		color = LiPath(ray, nullptr);
		break;
	}
	film.AddPixel(x, y, glm::vec4(color, 1.0));
}

Vec3 CPURayTracer::TraceNormals(Ray ray) const {
	RTInteraction intr;
	scene->Intersect(ray, intr);
	return glm::abs(intr.n);
}

Vec3 CPURayTracer::TraceRay(Ray ray) const {
	Vec3 L = Vec3(0);
	Vec3 beta = Vec3(1);
	int32_t depth = 0;
	bool specularBounce = true;

	while (true) {
		RTInteraction intr;
		bool intersected = scene->Intersect(ray, intr);
		if (!intersected) {
			L += scene->GetSkyColor(ray) * beta;
			break;
		}

		if (depth++ > maxDepth) {
			break;
		}

		if (!sampleLights || specularBounce) {
			L += intr.material->emission * beta;
		}

		Ray scatteredRay;
		Vec3 brdfMultiplier = intr.material->Sample(ray, intr, scatteredRay);
		beta *= brdfMultiplier;

		if (sampleLights && intr.material->emission == Vec3(0) && intr.material->transparency == 0) {
			std::optional<SampledLight> sampledLight = lightSampler->Sample(RandomFloat());
			if (sampledLight) {
				std::optional<LightLiSample> ls = sampledLight->light.SampleLi(intr, Vec2(RandomFloat(), RandomFloat()));
				if (ls && ls->L != Vec3(0) && ls->pdf > 0) {
					if (Unoccluded(intr, ls->pLight)) {
						L += beta * ls->L * glm::abs(glm::dot(ls->wi, intr.n)) / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (MaxComponent(beta) < 1 && depth > 1) {
			Float q = std::max<Float>(0, 1 - MaxComponent(beta));
			if (RandomFloat() < q)
				break;
			beta /= 1 - q;
		}

		ray = scatteredRay;
		specularBounce = false;
	}

	return L;
}

Vec3 CPURayTracer::TracePath(Ray ray) const {
	RTInteraction intr;
	Vec3 L = Vec3(0);
	Vec3 beta = Vec3(1);

	for (int32_t depth = 0; depth < maxDepth; depth++) {
		bool intersected = scene->Intersect(ray, intr);

		if (!intersected) {
			L += scene->GetSkyColor(ray) * beta;
			break;
		}

		L += intr.material->emission * beta;

		BSDF bsdf = intr.material->GetBSDF(intr);
		if (!bsdf) break;

		Vec2 u = Vec2(RandomFloat(), RandomFloat());
		Vec3 wo = -ray.d;
		Vec3 wp = SampleUniformSphere(u);

		Vec3 f = bsdf.f(wo, wp);
		Float dot = glm::abs(glm::dot(wp, intr.n));
		Vec3 fcos = f * dot / UniformSpherePDF();
		beta *= fcos;
		if (beta == Vec3(0)) break;

		ray = Ray(intr.p, wp);
	}

	return L;
}

Vec3 CPURayTracer::LiRandomWalk(Ray ray, int depth) const {
	RTInteraction isect;
	if (!scene->Intersect(ray, isect)) {
		return scene->GetSkyColor(ray);
	}

	Vec3 wo = -ray.d;

	Vec3 Le = isect.material->emission;

	if (depth == maxDepth)
		return Le;

	BSDF bsdf = isect.material->GetBSDF(isect);
	if (!bsdf)
		return Le;

	Vec2 u = Vec2(RandomFloat(), RandomFloat());
	Vec3 wp = SampleUniformSphere(u);

	Vec3 fcos = bsdf.f(wo, wp) * glm::abs(glm::dot(wp, isect.n));
	if (fcos == Vec3(0)) {
		return Le;
	}

	ray = Ray(isect.p, wp);
	return Le + fcos * LiRandomWalk(ray, depth + 1) / (1 / (4 * Pi));
}

Vec3 CPURayTracer::LiSimplePath(Ray ray) const {
	Vec3 L(0.0f), beta(1.0f);
	bool specularBounce = true;
	int depth = 0;
	while (beta != Vec3(0)) {
		RTInteraction isect;
		if (!scene->Intersect(ray, isect)) {
			L += beta * scene->GetSkyColor(ray);
			break;
		}

		if (!sampleLights || specularBounce)
			L += beta * isect.material->emission;

		if (depth++ == maxDepth)
			break;

		BSDF bsdf = isect.material->GetBSDF(isect);
		if (!bsdf) {
			specularBounce = true;
			ray = Ray(isect.p, ray.d);
			continue;
		}

		Vec3 wo = -ray.d;
		if (sampleLights) {
			std::optional<SampledLight> sampledLight = lightSampler->Sample(RandomFloat());
			if (sampledLight) {
				Vec2 uLight = Vec2(RandomFloat(), RandomFloat());
				std::optional<LightLiSample> ls = sampledLight->light.SampleLi(isect, uLight);
				if (ls && ls->L != Vec3(0) && ls->pdf > 0) {
					Vec3 wi = ls->wi;
					Vec3 f = bsdf.f(wo, wi) * glm::abs(glm::dot(wi, isect.n));
					if ((f != Vec3(0)) && Unoccluded(isect, ls->pLight)) {
						L += beta * f * ls->L / (sampledLight->p * ls->pdf);
					}
				}
			}
		}

		if (sampleBSDF) {
			Float u = RandomFloat();
			std::optional<BSDFSample> bs = bsdf.Sample_f(wo, u, Vec2(RandomFloat(), RandomFloat()));
			if (!bs)
				break;
			beta *= bs->f * glm::abs(glm::dot(bs->wi, isect.n)) / bs->pdf;
			specularBounce = bs->IsSpecular();
			ray = Ray(isect.p, bs->wi);
		}
		else {
			Float pdf;
			Vec3 wi;
			BxDFFlags flags = bsdf.Flags();
			if (IsReflective(flags) && IsTransmissive(flags)) {
				wi = SampleUniformSphere(Vec2(RandomFloat(), RandomFloat()));
				pdf = UniformSpherePDF();
			}
			else {
				wi = SampleUniformHemisphere(Vec2(RandomFloat(), RandomFloat()));
				pdf = UniformHemispherePDF();
				if (IsReflective(flags) && glm::dot(wo, isect.n) * glm::dot(wi, isect.n) < 0)
					wi = -wi;
				else if (IsTransmissive(flags) && glm::dot(wo, isect.n) * glm::dot(wi, isect.n) > 0)
					wi = -wi;
			}
			beta *= bsdf.f(wo, wi) * glm::abs(glm::dot(wi, isect.n)) / pdf;
			specularBounce = false;
			ray = Ray(isect.p, wi);
		}
	}
	return L;
}

Vec3 CPURayTracer::LiPath(Ray ray, VisibleSurface* visibleSurf) const {
	Vec3 L(0.0f), beta(1.0f);
	int32_t depth = 0;

	Float p_b = 1.0f, etaScale = 1.0f;
	bool specularBounce = false, anyNonSpecularBounces = false;
	RTInteraction prevIntrCtx;

	while (true) {
		RTInteraction isect;
		bool intersected = scene->Intersect(ray, isect);
		if (!intersected) {
			L += beta * scene->GetSkyColor(ray);
			break;
		}


		Vec3 Le = isect.material->emission;
		if (Le != Vec3(0.0f)) {
			if (depth == 0 || specularBounce)
				L += beta * Le;
			else {
				std::optional<SampledLight> sampledLight = lightSampler->Sample(RandomFloat());
				Float p_l = lightSampler->PMF();
				Float w_l = PowerHeuristic(1, p_b, 1, p_l);

				L += beta * w_l * Le;
			}
		}

		BSDF bsdf = isect.material->GetBSDF(isect);
		if (!bsdf) {
			specularBounce = true;
			ray = Ray(isect.p, ray.d);
			continue;
		}

		if (depth == 0 && visibleSurf) {
			constexpr int nRhoSamples = 16;
			const Float ucRho[nRhoSamples] = {
				0.75741637f, 0.37870818f, 0.7083487f, 0.18935409f, 0.9149363f, 0.35417435f,
				0.5990858f,  0.09467703f, 0.8578725f, 0.45746812f, 0.686759f,  0.17708716f,
				0.9674518f,  0.2995429f,  0.5083201f, 0.047338516f };
			const Vec2 uRho[nRhoSamples] = {
				Vec2(0.855985f, 0.570367f), Vec2(0.381823f, 0.851844f),
				Vec2(0.285328f, 0.764262f), Vec2(0.733380f, 0.114073f),
				Vec2(0.542663f, 0.344465f), Vec2(0.127274f, 0.414848f),
				Vec2(0.964700f, 0.947162f), Vec2(0.594089f, 0.643463f),
				Vec2(0.095109f, 0.170369f), Vec2(0.825444f, 0.263359f),
				Vec2(0.429467f, 0.454469f), Vec2(0.244460f, 0.816459f),
				Vec2(0.756135f, 0.731258f), Vec2(0.516165f, 0.152852f),
				Vec2(0.180888f, 0.214174f), Vec2(0.898579f, 0.503897f) };

			Vec3 albedo = bsdf.rho(isect.wo, ucRho, uRho);

			*visibleSurf = VisibleSurface(isect, albedo);
		}

		if (regularize && anyNonSpecularBounces) {
			bsdf.Regularize();
		}

		if (depth++ == maxDepth)
			break;

		if (IsNonSpecular(bsdf.Flags())) {
			Vec3 Ld = SampleLd(isect, &bsdf);
			L += beta * Ld;
		}

		Vec3 wo = -ray.d;
		Float u = RandomFloat();
		std::optional<BSDFSample> bs = bsdf.Sample_f(wo, u, Vec2(RandomFloat(), RandomFloat()));
		if (!bs)
			break;

		beta *= bs->f * glm::abs(glm::dot(bs->wi, isect.n)) / bs->pdf;
		p_b = bs->pdfIsProportional ? bsdf.PDF(wo, bs->wi) : bs->pdf;
		specularBounce = bs->IsSpecular();
		anyNonSpecularBounces |= !bs->IsSpecular();
		if (bs->IsTransmission())
			etaScale *= Sqr(bs->eta);
		prevIntrCtx = isect;

		ray = Ray(isect.p, bs->wi);

		Vec3 rrBeta = beta * etaScale;
		if (MaxComponent(rrBeta) < 1 && depth > 1) {
			Float q = std::max<Float>(0, 1 - MaxComponent(rrBeta));
			if (RandomFloat() < q)
				break;
			beta /= 1 - q;
		}
	}
	return L;
}

Vec3 CPURayTracer::SampleLd(const RTInteraction& intr, const BSDF* bsdf) const {
	LightSampleContext ctx(intr.p, intr.n);

	BxDFFlags flags = bsdf->Flags();
	if (IsReflective(flags) && !IsTransmissive(flags))
		ctx.p += intr.wo * ShadowEpsilon;
	else if (IsTransmissive(flags) && !IsReflective(flags))
		ctx.p -= intr.wo * ShadowEpsilon;

	Float u = RandomFloat();
	std::optional<SampledLight> sampledLight = lightSampler->Sample(u);
	Vec2 uLight = Vec2(RandomFloat(), RandomFloat());
	if (!sampledLight)
		return {};

	DiffuseAreaLight light = sampledLight->light;
	std::optional<LightLiSample> ls = light.SampleLi(intr, uLight);
	if (!ls || ls->L == Vec3(0) || ls->pdf == 0)
		return {};

	Vec3 wo = intr.wo, wi = ls->wi;
	Vec3 f = bsdf->f(wo, wi) * glm::abs(glm::dot(wi, intr.n));
	if (f == Vec3(0) || !Unoccluded(intr, ls->pLight))
		return {};

	Float p_l = sampledLight->p * ls->pdf;
	Float p_b = bsdf->PDF(wo, wi);
	Float w_l = PowerHeuristic(1, p_l, 1, p_b);
	return w_l * ls->L * f / p_l;
}
