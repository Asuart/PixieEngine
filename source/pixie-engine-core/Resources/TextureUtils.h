#pragma once
#include "pch.h"

enum class TextureWrap {
	Reapeat = 0,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
};

enum class TextureFiltering {
	Nearest,
	Linear,
	NearestMipmapNearest,
	LinearMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapLinear,
};

GLint glCastTextureWrap(TextureWrap wrap);
GLint glCastTextureFiltering(TextureFiltering filtering);
