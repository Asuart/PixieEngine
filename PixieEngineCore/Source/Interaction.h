#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Ray.h"
#include "Medium.h"

class Material;
class RTMaterial;
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
	Ray SpawnRayTo(Vec3 point) const;
	Ray SpawnRayTo(const Interaction& it) const;
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
	int32_t faceIndex = 0;
	Material* material = nullptr;
	Light* areaLight = nullptr;
};

struct MediumInteraction : public Interaction {

};

struct InteractionBase {
	Vec3 p = Vec3(0);
	Vec3 n = Vec3(0);
	Vec2 uv = Vec2(0);
	Float distance = 0;
	bool backFace = false;
};

struct RayInteraction : InteractionBase {
	Material* material = nullptr;
	Mesh* mesh = nullptr;
	int32_t triangleIndex = -1;
};

struct RTInteraction : InteractionBase {
	Vec3 wo = Vec3(0);
	Vec3 dpdus = Vec3(0);
	RTMaterial* material = 0;
	Shape* shape = nullptr;
	Float area = 0;
};
