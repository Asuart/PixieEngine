#pragma once
#include "pch.h"

struct BezierCurve2D {
	Vec2 p0, p1, p2, p3;

	BezierCurve2D(Vec2 _p0, Vec2 _p1, Vec2 _p2, Vec2 _p3) :
		p0(_p0), p1(_p1), p2(_p2), p3(_p3) {}

	inline Vec2 GetPoint(Float t) const {
		Float oneMinusT = 1.0f - t;
		Float oneMinusT2 = oneMinusT * oneMinusT;
		Float oneMinusT3 = oneMinusT2 * oneMinusT;
		Float t2 = t * t;
		Float t3 = t2 * t;
		return oneMinusT3 * p0 + 3 * oneMinusT2 * t * p1 + 3 * oneMinusT * t2 * p2 + t3 * p3;
	}

	inline void SetPoints(Vec2 _p0, Vec2 _p1, Vec2 _p2, Vec2 _p3) {
		p0 = _p0;
		p1 = _p1;
		p2 = _p2;
		p3 = _p3;
	}
};

struct BezierCurve3D {
	Vec3 p0, p1, p2, p3;

	BezierCurve3D(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3) :
		p0(_p0), p1(_p1), p2(_p2), p3(_p3) {}

	inline Vec3 GetPoint(Float t) const {
		Float oneMinusT = 1.0f - t;
		Float oneMinusT2 = oneMinusT * oneMinusT;
		Float oneMinusT3 = oneMinusT2 * oneMinusT;
		Float t2 = t * t;
		Float t3 = t2 * t;
		return oneMinusT3 * p0 + 3 * oneMinusT2 * t * p1 + 3 * oneMinusT * t2 * p2 + t3 * p3;
	}

	inline void SetPoints(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3) {
		p0 = _p0;
		p1 = _p1;
		p2 = _p2;
		p3 = _p3;
	}
};
