#include "pch.h"
#include "AntiAliasigUtils.h"

namespace PixieEngine {

std::string to_string(AntiAliasing mode) {
	switch (mode) {
	case AntiAliasing::None: return "None";
	case AntiAliasing::SSAAx2: return "SSAAx2";
	case AntiAliasing::SSAAx4: return "SSAAx4";
	case AntiAliasing::SSAAx8: return "SSAAx8";
	case AntiAliasing::MSAAx2: return "MSAAx2";
	case AntiAliasing::MSAAx4: return "MSAAx4";
	case AntiAliasing::MSAAx8: return "MSAAx8";
	case AntiAliasing::MSAAx16: return "MSAAx16";
	case AntiAliasing::FXAA: return "FXAA";
	default: return "Undefined";
	}
}

}