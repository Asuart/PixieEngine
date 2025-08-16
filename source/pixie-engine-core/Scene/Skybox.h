#pragma once
#include "pch.h"
#include "Resources/Cubemap.h"

namespace PixieEngine {

class Skybox {
public:
	Cubemap m_cubemapTexture;
	Cubemap m_lightmapTexture;
	Cubemap m_prefilteredTexture;

	Skybox();
	Skybox(Cubemap cubemapTexture, Cubemap lightmapTexture, Cubemap prefilteredTexture);
};

}