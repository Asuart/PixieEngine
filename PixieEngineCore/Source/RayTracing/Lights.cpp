#include "pch.h"
#include "Lights.h"

bool IsDeltaLight(LightType type) {
	return (type == LightType::DeltaPosition || type == LightType::DeltaDirection);
}

/*
	Light Sample Context
*/

LightSampleContext::LightSampleContext(const Vec3& position, const Vec3& normal) :
	position(position), normal(normal) {}

LightSampleContext::LightSampleContext(const RayInteraction& intr) :
	position(intr.position), normal(intr.normal) {}

/*
	Light Li Sample
*/

LightLiSample::LightLiSample(Spectrum L, Vec3 wi, Float pdf, const RayInteraction& pLight) :
	L(L), wi(wi), pdf(pdf), pLight(pLight) {}

LightLeSample::LightLeSample(Spectrum spectrum, Ray ray, Float pPosition, Float pDirection) :
	spectrum(spectrum), ray(ray), pPosition(pPosition), pDirection(pDirection) {}

/*
	Light Le Sample
*/

LightLeSample::LightLeSample(Spectrum spectrum, Ray ray, RayInteraction intr, Float pPosition, Float pDirection) :
	spectrum(spectrum), ray(ray), intr(intr), pPosition(pPosition), pDirection(pDirection) {}

/*
	Light
*/

Light::Light(LightType type, const Transform& transform) :
	m_type(type), m_transform(transform) {}

Spectrum Light::L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const {
	return Spectrum();
}

Spectrum Light::Le(const Ray& ray) const {
	return Spectrum();
}

LightType Light::Type() const {
	return m_type;
}

void Light::Preprocess(const Bounds3f& sceneBounds) {}

const Transform& Light::GetTransform() {
	return m_transform;
}

/*
	Point Light
*/

PointLight::PointLight(Spectrum spectrum, Float scale, const Transform& transform) :
	Light(LightType::DeltaPosition, transform), m_spectrum(spectrum), m_scale(scale) {}

Spectrum PointLight::Phi() const {
	return 4 * Pi * m_scale * m_spectrum;
}

std::optional<LightLiSample> PointLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	Vec3 p = m_transform.ApplyPoint(Vec3(0.0f, 0.0f, 0.0f));
	Vec3 wi = glm::normalize(p - context.position);
	Spectrum Li = m_scale * m_spectrum / glm::length2(p - context.position);
	return LightLiSample(Li, wi, 1, RayInteraction(p));
}

Float PointLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
	return 0;
}

std::optional<LightLeSample> PointLight::SampleLe(Vec2 u1, Vec2 u2) const {
	Vec3 p = m_transform.ApplyPoint(Vec3(0.0f));
	Ray ray(p, SampleUniformSphere(u1));
	return LightLeSample(m_scale * m_spectrum, ray, 1, UniformSpherePDF);
}

void PointLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	*pdfPos = 0;
	*pdfDir = UniformSpherePDF;
}

void PointLight::SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	std::cout << "SampleLePDF Shouldn't be called for non-area lights.\n";
}

Bounds3f PointLight::Bounds() const {
	return Bounds3f();
}

/*
	Spot Light
*/

SpotLight::SpotLight(Spectrum spectrum, Float scale, Float totalWidth, Float falloffStart, const Transform& transform) :
	Light(LightType::DeltaPosition, transform), m_spectrum(spectrum), m_scale(scale), m_cosFalloffStart(falloffStart), m_cosFalloffEnd(totalWidth) {}

Spectrum SpotLight::Phi() const {
	return m_scale * m_spectrum * 2.0f * Pi * ((1.0f - m_cosFalloffStart) + (m_cosFalloffStart - m_cosFalloffEnd) / 2.0f);
}

std::optional<LightLiSample> SpotLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	Vec3 p = m_transform.ApplyPoint(Vec3(0.0f));
	Vec3 wi = glm::normalize(p - context.position);
	Vec3 wLight = glm::normalize(m_transform.ApplyInverseVector(-wi));
	Spectrum Li = I(wLight) / length2(p - context.position);

	if (!Li) {
		return {};
	}
	return LightLiSample(Li, wi, 1.0f, RayInteraction(p));
}

Float SpotLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
	return 0;
}

Spectrum SpotLight::I(Vec3 w) const {
	return SmoothStep(CosTheta(w), m_cosFalloffEnd, m_cosFalloffStart) * m_scale * m_spectrum;
}

/*
	Directional Light
*/

DirectionalLight::DirectionalLight(Spectrum spectrum, Float scale, const Transform& transform) :
	Light(LightType::DeltaDirection, transform), m_spectrum(spectrum), m_scale(scale) {}

Spectrum DirectionalLight::Phi() const {
	return m_scale * m_spectrum * Pi * Sqr(m_sceneRadius);
}

std::optional<LightLiSample> DirectionalLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	Vec3 wi = glm::normalize(m_transform.ApplyVector(Vec3(0.0f, 0.0f, 1.0f)));
	Vec3 pOutside = context.position + wi * ((Float)2 * m_sceneRadius);
	return LightLiSample(m_scale * m_spectrum, wi, 1.0f, RayInteraction(pOutside));
}

Float DirectionalLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
	return 0.0f;
}

std::optional<LightLeSample> DirectionalLight::SampleLe(Vec2 u1, Vec2 u2) const {
	Vec3 w = glm::normalize(m_transform.ApplyInverseVector(Vec3(0, 0, 1)));
	Frame wFrame = Frame::FromZ(w);
	Vec2 cd = SampleUniformDiskConcentric(u1);
	Float sceneRadius = 5.0f;
	Vec3 pDisk = sceneRadius * wFrame.FromLocal(Vec3(cd.x, cd.y, 0));

	// Compute _DistantLight_ light ray
	Ray ray(pDisk + sceneRadius * w, -w);

	return LightLeSample(m_scale, ray, 1.0f / (Pi * Sqr(sceneRadius)), 1.0f);
}

void DirectionalLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	Float sceneRadius = 5.0f;
	*pdfPos = 1.0f / (Pi * sceneRadius * sceneRadius);
	*pdfDir = 0;
}

void DirectionalLight::SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	std::cout << "Shouldn't be called for non-area lights\n";
}

Bounds3f DirectionalLight::Bounds() const {
	return Bounds3f();
}

void DirectionalLight::Preprocess(const Bounds3f& sceneBounds) {
	sceneBounds.BoundingSphere(&m_sceneCenter, &m_sceneRadius);
	if (m_sceneRadius < ShadowEpsilon) {
		m_sceneRadius = 100.0f;
	}
}

/*
	Diffuse Area Light
*/

DiffuseAreaLight::DiffuseAreaLight(const Shape* shape, Transform transform, int32_t materialIndex) :
	Light(LightType::Area, transform), m_shape(shape), m_materialIndex(materialIndex) {}

Spectrum DiffuseAreaLight::Phi() const {
	return Pi * 2.0f * m_shape->Area() * ResourceManager::GetMaterial(m_materialIndex)->GetEmission();
}

Spectrum DiffuseAreaLight::L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const {
	return ResourceManager::GetMaterial(m_materialIndex)->GetEmission();
}

std::optional<LightLiSample> DiffuseAreaLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	ShapeSampleContext shapeContext = ShapeSampleContext(context.position, context.normal);
	std::optional<ShapeSample> ss = m_shape->Sample(shapeContext, u);
	if (!ss || ss->pdf == 0) {
		return {};
	}
	Vec3 offset = ss->intr.position - context.position;
	if (length2(offset) < ShadowEpsilon) {
		return {};
	}
	Vec3 wi = glm::normalize(offset);
	Spectrum Le = L(ss->intr.position, ss->intr.normal, ss->intr.uv, -wi);
	if (!Le) {
		return {};
	}
	return LightLiSample(Le, wi, ss->pdf, ss->intr);
}

Float DiffuseAreaLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
	ShapeSampleContext shapeCtx(context.position, context.normal);
	return m_shape->SamplePDF(shapeCtx, wi);
}

Spectrum DiffuseAreaLight::Le(const Ray& ray) const {
	return ResourceManager::GetMaterial(m_materialIndex)->GetEmission();
}

std::optional<LightLeSample> DiffuseAreaLight::SampleLe(Vec2 u1, Vec2 u2) const {
	std::optional<ShapeSample> ss = m_shape->Sample(u1);
	if (!ss) {
		return {};
	}

	Vec3 w;
	Float pdfDir;
	if (u2[0] < 0.5f) {
		u2[0] = std::min(u2[0] * 2, OneMinusEpsilon);
		w = SampleCosineHemisphere(u2);
	}
	else {
		u2[0] = std::min((u2[0] - 0.5f) * 2, OneMinusEpsilon);
		w = SampleCosineHemisphere(u2);
		w.z *= -1;
	}
	pdfDir = CosineHemispherePDF(std::abs(w.z)) / 2;
	if (pdfDir == 0) {
		return {};
	}

	const RayInteraction& intr = ss->intr;
	Frame nFrame = Frame::FromZ(intr.normal);
	w = nFrame.FromLocal(w);
	Spectrum Le = L(intr.position, intr.normal, intr.uv, w);
	return LightLeSample(Le, Ray(intr.position, w), intr, ss->pdf, pdfDir);
}

void DiffuseAreaLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	std::cout << "ERROR: shouldn't be called for area lights.\n";
	exit(1);
}

void DiffuseAreaLight::SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	*pdfPos = m_shape->SamplePDF(intr);
	*pdfDir = CosineHemispherePDF(AbsDot(intr.normal, w)) / 2.0f;
}

Bounds3f DiffuseAreaLight::Bounds() const {
	return m_shape->Bounds();
}

/*
	Uniform Infinite Lights
*/

UniformInfiniteLight::UniformInfiniteLight(Spectrum spectrum, Float scale, const Transform& transform) :
	Light(LightType::Infinite, transform), m_spectrum(spectrum), m_scale(scale) {}

Spectrum UniformInfiniteLight::Phi() const {
	return 4 * Pi * Pi * Sqr(m_sceneRadius) * m_scale * m_spectrum;
}

std::optional<LightLiSample> UniformInfiniteLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	if (allowIncompletePDF) {
		return {};
	}
	Vec3 wi = SampleUniformSphere(u);
	Float pdf = UniformSpherePDF;
	return LightLiSample(m_scale * m_spectrum, wi, pdf, RayInteraction(context.position + wi * (Float)2 * m_sceneRadius));
}

Float UniformInfiniteLight::SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF) const {
	return allowIncompletePDF ? 0 : UniformSpherePDF;
}

Spectrum UniformInfiniteLight::Le(const Ray& ray) const {
	return m_scale * m_spectrum;
}

std::optional<LightLeSample> UniformInfiniteLight::SampleLe(Vec2 u1, Vec2 u2) const {
	Vec3 w = SampleUniformSphere(u1);

	Frame wFrame = Frame::FromZ(-w);
	Vec2 cd = SampleUniformDiskConcentric(u2);
	Vec3 pDisk = m_sceneCenter + m_sceneRadius * wFrame.FromLocal(Vec3(cd.x, cd.y, 0));
	Ray ray(pDisk + m_sceneRadius * -w, w);

	Float pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
	Float pdfDir = UniformSpherePDF;

	return LightLeSample(m_scale * m_spectrum, ray, pdfPos, pdfDir);
}

void UniformInfiniteLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	*pdfDir = UniformSpherePDF;
	*pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
}

void UniformInfiniteLight::SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	std::cout << "Shouldn't be called for non-area lights\n";
}

Bounds3f UniformInfiniteLight::Bounds() const {
	return Bounds3f();
}

void UniformInfiniteLight::Preprocess(const Bounds3f& sceneBounds) {
	sceneBounds.BoundingSphere(&m_sceneCenter, &m_sceneRadius);
}

/*
	Gradient Infinite Color
*/

GradientInfiniteLight::GradientInfiniteLight(Spectrum topSpectrum, Spectrum bottomSpectrum, Float scale, const Transform& transform) :
	Light(LightType::Infinite, transform), m_topSpectrum(topSpectrum), m_bottomSpectrum(bottomSpectrum), m_scale(scale) {}

Spectrum GradientInfiniteLight::Phi() const {
	return 4 * Pi * Pi * Sqr(m_sceneRadius) * m_scale * (m_topSpectrum + m_bottomSpectrum) / 2.0f;
}

std::optional<LightLiSample> GradientInfiniteLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	if (allowIncompletePDF) {
		return {};
	}
	Vec3 wi = SampleUniformSphere(u);
	Float pdf = UniformSpherePDF;
	return LightLiSample(GetSpectrum(wi), wi, pdf, RayInteraction(context.position + wi * (Float)2 * m_sceneRadius));
}

Float GradientInfiniteLight::SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF) const {
	return allowIncompletePDF ? 0 : UniformSpherePDF;
}

Spectrum GradientInfiniteLight::Le(const Ray& ray) const {
	return GetSpectrum(ray.direction);
}

std::optional<LightLeSample> GradientInfiniteLight::SampleLe(Vec2 u1, Vec2 u2) const {
	Vec3 w = SampleUniformSphere(u1);

	Frame wFrame = Frame::FromZ(-w);
	Vec2 cd = SampleUniformDiskConcentric(u2);
	Vec3 pDisk = m_sceneCenter + m_sceneRadius * wFrame.FromLocal(Vec3(cd.x, cd.y, 0));
	Ray ray(pDisk + m_sceneRadius * -w, w);

	Float pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
	Float pdfDir = UniformSpherePDF;

	return LightLeSample(GetSpectrum(ray.direction), ray, pdfPos, pdfDir);
}

void GradientInfiniteLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	*pdfDir = UniformSpherePDF;
	*pdfPos = 1 / (Pi * Sqr(m_sceneRadius));
}

void GradientInfiniteLight::SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	std::cout << "Shouldn't be called for non-area lights\n";
}

Bounds3f GradientInfiniteLight::Bounds() const {
	return Bounds3f();
}

void GradientInfiniteLight::Preprocess(const Bounds3f& sceneBounds) {
	sceneBounds.BoundingSphere(&m_sceneCenter, &m_sceneRadius);
}

Spectrum GradientInfiniteLight::GetSpectrum(Vec3 direction) const {
	Float t = (direction.y + 1.0f) / 2.0f;
	t = t * t;
	return (m_topSpectrum * t + m_bottomSpectrum * (1.0f - t)) * m_scale;
}

/*
	Image infinite light
*/

ImageInfiniteLight::ImageInfiniteLight(Transform renderFromLight, Buffer2D<Spectrum> image, Float scale) :
	Light(LightType::Infinite, renderFromLight), image(image), scale(scale) {
	Buffer2D<Float> d(image.GetResolution());
	Float average = 0.0f;
	for (size_t i = 0; i < image.GetSize(); i++) {
		d.m_data[i] = image.m_data[i].Average();
		average += d.m_data[i];
	}
	average /= (Float)image.GetSize();
	Bounds2f domain = Bounds2f(Vec2(0, 0), Vec2(1, 1));
	distribution = PiecewiseConstant2D(d, domain);
	for (size_t i = 0; i < d.GetSize(); i++) {
		d.m_data[i] = glm::max(d.m_data[i] - average, 0.0f);
	}
	bool allZero = true;
	for (size_t i = 0; i < d.GetSize(); i++) {
		if (d.m_data[i] > 0) {
			allZero = false;
			break;
		}
	}
	if (allZero) {
		for (size_t i = 0; i < d.GetSize(); i++) {
			d.m_data[i] = 1.0f;
		}
	}
	compensatedDistribution = PiecewiseConstant2D(d, domain);
}

void ImageInfiniteLight::Preprocess(const Bounds3f& sceneBounds) {
	sceneBounds.BoundingSphere(&sceneCenter, &sceneRadius);
}

Spectrum ImageInfiniteLight::Phi() const {
	Spectrum sumL(0.0f);
	int32_t width = image.GetResolution().x;
	int32_t height = image.GetResolution().y;
	for (int32_t v = 0; v < height; v++) {
		for (int32_t u = 0; u < width; u++) {
			sumL += image.GetValue(v, u);
		}
	}
	return 4 * Pi * Pi * Sqr(sceneRadius) * scale * sumL / (Float)(width * height);
}


Float ImageInfiniteLight::SampleLiPDF(LightSampleContext, Vec3 w, bool allowIncompletePDF) const {
	Vec3 wLight = m_transform.ApplyInverseVector(w);
	Vec2 uv = EqualAreaSphereToSquare(wLight);
	Float pdf = 0;
	if (allowIncompletePDF) {
		pdf = compensatedDistribution.PDF(uv);
	}
	else {
		pdf = distribution.PDF(uv);
	}
	return pdf / (4.0f * Pi);
}


std::optional<LightLeSample> ImageInfiniteLight::SampleLe(Vec2 u1, Vec2 u2) const {
	Float mapPDF;
	std::optional<Vec2> uv = distribution.Sample(u1, &mapPDF);
	if (!uv) {
		return {};
	}
	Vec3 wLight = EqualAreaSquareToSphere(*uv);
	Vec3 w = -m_transform.ApplyVector(wLight);

	Frame wFrame = Frame::FromZ(-w);
	Vec2 cd = SampleUniformDiskConcentric(u2);
	Vec3 pDisk = sceneCenter + sceneRadius * wFrame.FromLocal(Vec3(cd.x, cd.y, 0));
	Ray ray(pDisk + (sceneRadius * -w), w);

	Float pdfDir = mapPDF / (4 * Pi);
	Float pdfPos = 1 / (Pi * Sqr(sceneRadius));

	return LightLeSample(ImageLe(*uv), ray, pdfPos, pdfDir);
}

void ImageInfiniteLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	Vec3 wl = -m_transform.ApplyInverseVector(ray.direction);
	Float mapPDF = distribution.PDF(EqualAreaSphereToSquare(wl));
	*pdfDir = mapPDF / (4.0f * Pi);
	*pdfPos = 1.0f / (Pi * Sqr(sceneRadius));
}

void ImageInfiniteLight::SampleLePDF(const RayInteraction&, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	throw new std::exception("Shouldn't be called for non-area lights");
}

Spectrum ImageInfiniteLight::Le(const Ray& ray) const {
	Vec3 wLight = glm::normalize(m_transform.ApplyInverseVector(ray.direction));
	Vec2 uv = EqualAreaSphereToSquare(wLight);
	return ImageLe(uv);
}

std::optional<LightLiSample> ImageInfiniteLight::SampleLi(LightSampleContext ctx, Vec2 u, bool allowIncompletePDF) const {
	Float mapPDF = 0;
	Vec2 uv;
	if (allowIncompletePDF) {
		uv = compensatedDistribution.Sample(u, &mapPDF);
	}
	else {
		uv = distribution.Sample(u, &mapPDF);
	}
	if (mapPDF == 0) {
		return {};
	}

	Vec3 wLight = EqualAreaSquareToSphere(uv);
	Vec3 wi = m_transform.ApplyVector(wLight);

	Float pdf = mapPDF / (4.0f * Pi);

	return LightLiSample(ImageLe(uv), wi, pdf, RayInteraction(ctx.position + wi * (2 * sceneRadius)));
}

Bounds3f ImageInfiniteLight::Bounds() const {
	return {};
}

Spectrum ImageInfiniteLight::ImageLe(Vec2 uv) const {
	Spectrum rgb = image.GetValue(glm::ivec2(uv * Vec2(image.GetResolution() - glm::ivec2(1, 1))));
	return scale * rgb;
}