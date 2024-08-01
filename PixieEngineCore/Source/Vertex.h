#pragma once
#include "PixieEngineCoreHeaders.h"

static const uint32_t MaxBonesPerVertex = 4;

struct Vertex {
	Vec3 p;
	Vec3 n;
	Vec2 uv;
	int32_t boneIDs[MaxBonesPerVertex];
	Float boneWeights[MaxBonesPerVertex];
	Vertex(const Vec3& _p = Vec3(0), const Vec3& _n = Vec3(0), const Vec2& _uv = Vec2(0));
};
