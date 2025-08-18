#pragma once
#include "pch.h"

namespace PixieEngine {

struct ShaderModulesVulkan {
	VkShaderModule vertex;
	VkShaderModule fragment;
};

VkShaderModule CreateShaderModule(const std::string& source, const std::string& fileName, uint32_t stage);

ShaderModulesVulkan CompileShaderVulkan(const std::string& vertexShaderSource, const std::string& framgentShaderSource);

}