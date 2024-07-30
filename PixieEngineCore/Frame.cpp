#include "pch.h"
#include "Frame.h"

Frame::Frame()
	: x(1, 0, 0), y(0, 1, 0), z(0, 0, 1) {}

Frame::Frame(glm::vec3 x, glm::vec3 y, glm::vec3 z)
	: x(x), y(y), z(z) {}

Frame Frame::FromXZ(glm::vec3 x, glm::vec3 z) {
	return Frame(x, glm::cross(z, x), z);
}

Frame Frame::FromXY(glm::vec3 x, glm::vec3 y) {
	return Frame(x, y, glm::cross(x, y));
}

Frame Frame::FromZ(glm::vec3 z) {
	glm::vec3 x, y;
	CoordinateSystem(z, &x, &y);
	return Frame(x, y, z);
}

Frame Frame::FromX(glm::vec3 x) {
	glm::vec3 y, z;
	CoordinateSystem(x, &y, &z);
	return Frame(x, y, z);
}

Frame Frame::FromY(glm::vec3 y) {
	glm::vec3 x, z;
	CoordinateSystem(y, &z, &x);
	return Frame(x, y, z);
}

glm::vec3 Frame::ToLocal(glm::vec3 v) const {
	return glm::vec3(glm::dot(v, x), glm::dot(v, y), glm::dot(v, z));
}

glm::vec3 Frame::FromLocal(glm::vec3 v) const {
	return v.x * x + v.y * y + v.z * z;
}