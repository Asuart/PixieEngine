#pragma once
#include "pch.h"

namespace PixieEngine {

enum class RenderAPI : uint32_t {
	Undefined = 0,
	OpenGL,
	Vulkan,
	COUNT
};

inline std::string_view to_string(RenderAPI api) {
	switch (api) {
	case RenderAPI::OpenGL: return "OpenGL";
	case RenderAPI::Vulkan: return "Vulkan";
	}
	return "Unhandled Render API";
}

}