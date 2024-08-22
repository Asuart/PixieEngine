#pragma once
#include "pch.h"
#include "Medium.h"
#include "BSDF.h"
#include "Sampler.h"

struct Material;
class Shape;
class Camera;
struct Ray;
struct Mesh;
struct Light;
class AreaLight;
struct ShapeSample;
struct TriangleCache;

struct Interaction {
	Vec3 position = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f);
	Vec2 uv = Vec2(0.0f);
	Vec3 wo = Vec3(0.0f);
	Float distance = 0.0f;
	bool backface = false;
	const TriangleCache* triangle = nullptr;
	const Medium* medium = nullptr;
	const MediumInterface* mediumInterface = nullptr;

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
	Float area = 0.0f;
	int32_t faceIndex = 0;
	const Material* material = nullptr;
	const AreaLight* areaLight = nullptr;
	Mesh* mesh = nullptr;

	Spectrum Le(const glm::vec3& wo) const;
	BSDF GetBSDF(const Ray& ray, const Camera* camera, Sampler* sampler);
	Ray SpawnRay(const Ray& rayi, const BSDF& bsdf, Vec3 wi, BxDFFlags flags, Float eta) const;
	void SkipIntersection(Ray& ray) const;
};

struct MediumInteraction : public Interaction {

};
