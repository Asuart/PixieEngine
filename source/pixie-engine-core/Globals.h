#pragma once
#include "pch.h"

class Globals {
public:
	static void SetApplicationPath(const std::filesystem::path& path);
	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path GetAssetsPath();
	static void SetAssetsPath(const std::filesystem::path& path);

protected:
	static std::filesystem::path m_applicationPath;
	static std::filesystem::path m_currentFilePath;
	static std::filesystem::path m_assetsPath;
};
