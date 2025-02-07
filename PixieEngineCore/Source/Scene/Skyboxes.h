#pragma once
#include "pch.h"
#include "Buffer2DTexture.h"

class HDRISkybox {
public:
	Buffer2DTexture<Vec3> m_equrectangularTexture;
	Texture m_cubemapTexture;
	Texture m_lightmapTexture;
	Texture m_prefilteredTexture;

	HDRISkybox() = default;
	HDRISkybox(Buffer2DTexture<Vec3>& equrectangularTexture, Texture cubemapTexture, Texture lightmapTexture, Texture prefilteredTexture) :
		m_equrectangularTexture(equrectangularTexture), m_cubemapTexture(cubemapTexture), m_lightmapTexture(lightmapTexture), m_prefilteredTexture(prefilteredTexture) {}
};
