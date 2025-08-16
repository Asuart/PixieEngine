#pragma once
#include "pch.h"

namespace PixieEngine {

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

constexpr GLint glCastTextureWrap(TextureWrap wrap) {
	switch (wrap) {
	case TextureWrap::Reapeat: return GL_REPEAT;
	case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
	case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
	default: return GL_REPEAT;
	}
}

constexpr GLint glCastTextureFiltering(TextureFiltering filtering) {
	switch (filtering) {
	case TextureFiltering::Linear: return GL_LINEAR;
	case TextureFiltering::Nearest: return GL_NEAREST;
	case TextureFiltering::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureFiltering::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
	case TextureFiltering::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
	case TextureFiltering::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
	default: return GL_LINEAR;
	}
}

}