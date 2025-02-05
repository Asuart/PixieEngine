#include "pch.h"
#include "AssetsBrowserWindow.h"
#include "PixieEngineApp.h"

AssetsBrowserWindow::AssetsBrowserWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void AssetsBrowserWindow::Initialize() {
	m_folderIcon = ResourceManager::LoadTexture("folder-icon.png");
	m_parentFolderIcon = ResourceManager::LoadTexture("folder-up-icon.png");
	m_fileIcon = ResourceManager::LoadTexture("file-icon.png");
}

void AssetsBrowserWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Assets Browser", 0)) {
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float cellSize = m_thumbnailSize + m_thumbnailPadding;
		int32_t columsCount = glm::max((int32_t)(availableWidth / cellSize), 1);

		ImGui::Columns(columsCount, 0, false);

		std::filesystem::path assetPath = ResourceManager::GetAssetsPath();

		if (assetPath.parent_path() != "") {
			ImGui::ImageButton((ImTextureID)(uint64_t)m_parentFolderIcon.m_id, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				ResourceManager::SetAssetsPath(assetPath.parent_path());
			}
			ImGui::TextWrapped("../");
			ImGui::NextColumn();
		}

		std::vector<std::filesystem::path> directories;
		std::vector<std::filesystem::path> files;
		for (const auto entry : std::filesystem::directory_iterator(assetPath)) {
			const std::filesystem::path& path = entry.path();
			if (entry.is_directory()) {
				directories.push_back(path);
			} 
			else {
				files.push_back(path);
			}
		}

		for (const auto& path : directories) {
			const std::string fileName = path.filename().string();
			ImGui::ImageButton((ImTextureID)(uint64_t)m_folderIcon.m_id, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				ResourceManager::SetAssetsPath(assetPath.string() + std::string("/") + fileName);
			}
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}

		for (const auto& path : files) {
			const std::string fileName = path.filename().string();
			ImGui::ImageButton((ImTextureID)(uint64_t)m_fileIcon.m_id, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (ResourceManager::IsValidScenePath(path)) {
					SceneManager::LoadScene(path);
				}
				else if (ResourceManager::IsValidModelPath(path)) {
					SceneManager::LoadModel(path);
				}
			}
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}
	ImGui::End();
}
