#include "pch.h"
#include "Globals.h"

std::filesystem::path Globals::m_applicationPath = "";
std::filesystem::path Globals::m_currentFilePath = "";
std::filesystem::path Globals::m_assetsPath = "/resources/scenes/";

void Globals::SetApplicationPath(const std::filesystem::path& path) {
	m_applicationPath = path;
}

std::filesystem::path Globals::GetApplicationPath() {
	return m_applicationPath;
}

std::filesystem::path Globals::GetApplicationDirectory() {
	return m_applicationPath.parent_path();
}

std::filesystem::path Globals::GetAssetsPath() {
	return GetApplicationDirectory().string() + m_assetsPath.string();
}

void Globals::SetAssetsPath(const std::filesystem::path& path) {
	m_assetsPath = path;
}
