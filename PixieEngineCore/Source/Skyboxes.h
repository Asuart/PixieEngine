#pragma once
#include "pch.h"

enum class SkyboxType : int32_t {
	SolidColor = 0,
	GradientColor,
};

class Skybox {
public:
	virtual void Draw() {}
};

class SolidColorSkybox : public Skybox {
public:
	Vec3 m_color;

	SolidColorSkybox(Vec3 color) :
		m_color(color) {}
};

class GradientColorSkybox : public Skybox {
public:
	Vec3 m_topColor;
	Vec3 m_bottomColor;

	GradientColorSkybox(Vec3 topColor, Vec3 bottomColor) :
		m_topColor(topColor), m_bottomColor(bottomColor) {}
};
