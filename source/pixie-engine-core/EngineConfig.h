#pragma once
#include "pch.h"

namespace PixieEngine {

class EngineConfig {
public:
	static void SetApplicationPath(const std::filesystem::path& path);
	static void SetAssetsPath(const std::filesystem::path& path);

	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path GetAssetsPath();

protected:
	inline static std::filesystem::path m_applicationPath = "";
	inline static std::filesystem::path m_currentFilePath = "";
	inline static std::filesystem::path m_assetsPath = "";
};

}