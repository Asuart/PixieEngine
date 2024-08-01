#pragma once
#include "RTMath.h"

class Frame {
public:
	Vec3 x, y, z;

	Frame();
	Frame(Vec3 x, Vec3 y, Vec3 z);
	static Frame FromXZ(Vec3 x, Vec3 z);
	static Frame FromXY(Vec3 x, Vec3 y);
	static Frame FromZ(Vec3 z);
	static Frame FromX(Vec3 x);
	static Frame FromY(Vec3 y);

	Vec3 ToLocal(Vec3 v) const;
	Vec3 FromLocal(Vec3 v) const;
};
