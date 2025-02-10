#pragma once
#include "pch.h"
#include "Buffer2DTexture.h"

class HDRISkybox {
public:
	Buffer2DTexture<Vec3> m_equrectangularTexture;
	Cubemap m_cubemapTexture;
	Cubemap m_lightmapTexture;
	Cubemap m_prefilteredTexture;

	HDRISkybox() :
		m_cubemapTexture({ 512, 512 }), m_lightmapTexture({ 32, 32 }), m_prefilteredTexture({ 128, 128 }) {};
	HDRISkybox(Buffer2DTexture<Vec3>& equrectangularTexture, Cubemap cubemapTexture, Cubemap lightmapTexture, Cubemap prefilteredTexture) :
		m_equrectangularTexture(equrectangularTexture), m_cubemapTexture(cubemapTexture), m_lightmapTexture(lightmapTexture), m_prefilteredTexture(prefilteredTexture) {}
};
