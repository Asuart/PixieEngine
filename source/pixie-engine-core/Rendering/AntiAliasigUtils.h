#pragma once
#include "pch.h"

enum class AntiAliasing {
	None = 0,
	SSAAx2,
	SSAAx4,
	SSAAx8,
	MSAAx2,
	MSAAx4,
	MSAAx8,
	MSAAx16,
	FXAA,
	COUNT,
};

std::string to_string(AntiAliasing mode);
