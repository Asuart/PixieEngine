#pragma once
#include "pch.h"

namespace PixieEngine {

enum class TextureWrap {
	Reapeat,
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

enum class TextureFormat {
	Red,
	RGB,
	RGBA
};

}