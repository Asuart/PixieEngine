#pragma once
#include "PixieEngineCoreHeaders.h"

class Material;
class RTMaterial;
class Shape;
class Mesh;

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
