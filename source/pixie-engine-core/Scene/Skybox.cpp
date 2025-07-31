#include "pch.h"
#include "Skybox.h"

Skybox::Skybox() :
	m_cubemapTexture({ 512, 512 }), m_lightmapTexture({ 32, 32 }), m_prefilteredTexture({ 128, 128 }) {};

Skybox::Skybox(Cubemap cubemapTexture, Cubemap lightmapTexture, Cubemap prefilteredTexture) :
	m_cubemapTexture(cubemapTexture), m_lightmapTexture(lightmapTexture), m_prefilteredTexture(prefilteredTexture) {}
