#pragma once
#include "PixieEngineCoreHeaders.h"

class Sampler {
public:
	Sampler();

	virtual Float Get();
	virtual Vec2 Get2D();
	virtual Vec3 Get3D();

protected:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<> dist;
};