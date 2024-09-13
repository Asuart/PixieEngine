#pragma once
#include "pch.h"
#include "Ray.h"
#include "Medium.h"
#include "BSDF.h"
#include "Sampler.h"

class Camera;
struct Material;
class Light;

struct Interaction {
	Vec3 position = Vec3(0.0f);
	Vec3 wo = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f);
	Vec2 uv = Vec2(0.0f);
	const Medium* medium = nullptr;
	const MediumInterface* mediumInterface = nullptr;

	Interaction() = default;
	Interaction(Vec3 position, Vec3 normal, Vec2 uv, Vec3 wo = Vec3(0));
	Interaction(Vec3 position, const MediumInterface* mediumInterface);

	const Medium* GetMedium(Vec3 direction) const;
	const Medium* GetMedium() const;
	bool IsSurfaceInteraction() const;
	bool IsMediumInteraction() const;
};

struct SurfaceInteraction : public Interaction {
	Vec3 dpdu = Vec3(0.0f), dpdv = Vec3(0.0f);
	Vec3 dndu = Vec3(0.0f), dndv = Vec3(0.0f);
	Vec3 dpdx = Vec3(0.0f), dpdy = Vec3(0.0f);
	Float dudx = 0.0f, dvdx = 0.0f, dudy = 0.0f, dvdy = 0.0f;
	const Material* material = nullptr;
	const Light* areaLight = nullptr;

	SurfaceInteraction() = default;
	SurfaceInteraction(Vec3 position, Vec3 normal, Vec2 uv);
	SurfaceInteraction(Vec3 position, Vec2 uv, Vec3 wo, Vec3 dpdu, Vec3 dpdv, Vec3 dndu, Vec3 dndv);
	SurfaceInteraction(Vec3 position, const MediumInterface* mediumInterface);

	Spectrum Le(const glm::vec3& wo) const;
	BSDF GetBSDF(const Ray& ray, const Camera* camera, Sampler* sampler);
	Ray SpawnRay(const Ray& rayi, const BSDF& bsdf, Vec3 wi, BxDFFlags flags, Float eta) const;
	void SkipIntersection(Ray& ray) const;
};

struct MediumInteraction : public Interaction {

};
