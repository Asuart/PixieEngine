#pragma once
#include "pch.h"
#include "Resources/Cubemap.h"

class Skybox {
public:
	Cubemap m_cubemapTexture;
	Cubemap m_lightmapTexture;
	Cubemap m_prefilteredTexture;

	Skybox();
	Skybox(Cubemap cubemapTexture, Cubemap lightmapTexture, Cubemap prefilteredTexture);
};
