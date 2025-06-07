#include "pch.h"
#include "Globals.h"

std::filesystem::path Globals::m_applicationPath = "";
std::filesystem::path Globals::m_currentFilePath = "";
std::filesystem::path Globals::m_assetsPath = "../Assets/Scenes";

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
	return m_assetsPath;
}

void Globals::SetAssetsPath(const std::filesystem::path& path) {
	m_assetsPath = path;
}
