#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Ray.h"
#include "Medium.h"

class Material;
class Material;
class Shape;
class Mesh;
struct Light;

struct Interaction {
	Vec3 position = Vec3(0.0f);
	Vec3 wo = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f);
	Vec2 uv = Vec2(0.0f);
	Float distance = 0.0f;
	Medium* medium = nullptr;
	MediumInterface* mediumInterface = nullptr;
    bool backface = false;

	Vec3 OffsetRayOrigin(Vec3 origin) const;
	Medium* GetMedium(Vec3 direction) const;
	Medium* GetMedium() const;
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
	Material* material = nullptr;
	Mesh* mesh = nullptr;
	Light* areaLight = nullptr;
};

struct MediumInteraction : public Interaction {

};
