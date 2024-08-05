#include "Sampler.h"

Sampler::Sampler() 
	: gen(rd()), dist(0, 1) {}

Float Sampler::Get() {
	return (Float)dist(gen);
}

Vec2 Sampler::Get2D() {
	return Vec2(Get(), Get());
}

Vec3 Sampler::Get3D() {
	return Vec3(Get(), Get(), Get());
}