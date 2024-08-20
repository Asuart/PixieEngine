#include "pch.h"
#include "Frame.h"

Frame::Frame()
	: x(1, 0, 0), y(0, 1, 0), z(0, 0, 1) {}

Frame::Frame(Vec3 x, Vec3 y, Vec3 z)
	: x(x), y(y), z(z) {}

Frame Frame::FromXZ(Vec3 x, Vec3 z) {
	return Frame(x, glm::cross(z, x), z);
}

Frame Frame::FromXY(Vec3 x, Vec3 y) {
	return Frame(x, y, glm::cross(x, y));
}

Frame Frame::FromZ(Vec3 z) {
	Vec3 x, y;
	CoordinateSystem(z, &x, &y);
	return Frame(x, y, z);
}

Frame Frame::FromX(Vec3 x) {
	Vec3 y, z;
	CoordinateSystem(x, &y, &z);
	return Frame(x, y, z);
}

Frame Frame::FromY(Vec3 y) {
	Vec3 x, z;
	CoordinateSystem(y, &z, &x);
	return Frame(x, y, z);
}

Vec3 Frame::ToLocal(Vec3 v) const {
	return Vec3(glm::dot(v, x), glm::dot(v, y), glm::dot(v, z));
}

Vec3 Frame::FromLocal(Vec3 v) const {
	return v.x * x + v.y * y + v.z * z;
}