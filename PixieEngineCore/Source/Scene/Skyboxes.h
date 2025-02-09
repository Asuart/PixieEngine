#pragma once
#include "pch.h"
#include "Buffer2DTexture.h"

class HDRISkybox {
public:
	Buffer2DTexture<Vec3> m_equrectangularTexture;
	Cubemap m_cubemapTexture;
	Cubemap m_lightmapTexture;
	Cubemap m_prefilteredTexture;

	HDRISkybox() = default;
	HDRISkybox(Buffer2DTexture<Vec3>& equrectangularTexture, Cubemap cubemapTexture, Cubemap lightmapTexture, Cubemap prefilteredTexture) :
		m_equrectangularTexture(equrectangularTexture), m_cubemapTexture(cubemapTexture), m_lightmapTexture(lightmapTexture), m_prefilteredTexture(prefilteredTexture) {}
};
