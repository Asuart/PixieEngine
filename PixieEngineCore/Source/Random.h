#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTMath.h"

Float RandomFloat();
Float RandomFloat(Float min, Float max);

Vec3 RandomVector();
Vec3 RandomVector(Float min, Float max);
Vec3 RandomUnitVector();

Vec3 RandomInUnitSphere();
Vec3 RandomInHemisphere(const Vec3& normal);
Vec3 RandomInUnitDisk();
Vec3 RandomCosineDirection();
