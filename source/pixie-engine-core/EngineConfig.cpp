#include "pch.h"
#include "EngineConfig.h"

namespace PixieEngine {

void EngineConfig::SetApplicationPath(const std::filesystem::path& path) {
	m_applicationPath = path;
}

void EngineConfig::SetAssetsPath(const std::filesystem::path& path) {
	m_assetsPath = path;
}

std::filesystem::path EngineConfig::GetApplicationPath() {
	return m_applicationPath;
}

std::filesystem::path EngineConfig::GetApplicationDirectory() {
	return m_applicationPath.parent_path();
}

std::filesystem::path EngineConfig::GetAssetsPath() {
	return m_assetsPath;
}

}