#include "pch.h"
#include "AssetsBrowserWindow.h"
#include "PixieEngineApp.h"

AssetsBrowserWindow::AssetsBrowserWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter) {}

void AssetsBrowserWindow::Initialize() {
	m_folderIcon = ResourceManager::LoadRGBATexture("../Resources/folder-icon.png")->id;
	m_parentFolderIcon = ResourceManager::LoadRGBATexture("../Resources/folder-up-icon.png")->id;
	m_fileIcon = ResourceManager::LoadRGBATexture("../Resources/file-icon.png")->id;
}

void AssetsBrowserWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Assets Browser", 0)) {
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float cellSize = m_thumbnailSize + m_thumbnailPadding;
		int32_t columsCount = glm::max((int32_t)(availableWidth / cellSize), 1);

		ImGui::Columns(columsCount, 0, false);

		std::filesystem::path assetPath = m_app.GetAssetsPath();

		if (assetPath.parent_path() != "") {
			ImGui::ImageButton((ImTextureID)m_parentFolderIcon, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				m_app.SetAssetsPath(assetPath.parent_path());
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
			ImGui::ImageButton((ImTextureID)m_folderIcon, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				m_app.SetAssetsPath(assetPath.string() + "/" + fileName);
			}
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}

		for (const auto& path : files) {
			const std::string fileName = path.filename().string();
			ImGui::ImageButton((ImTextureID)m_fileIcon, { m_thumbnailSize, m_thumbnailSize }, { 0, 0 }, { 1, 1 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (ResourceManager::IsValidScenePath(path)) {
					m_app.LoadScene(path);
				}
				else if (ResourceManager::IsValidModelPath(path)) {
					m_app.LoadModel(path);
				}
			}
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}
	ImGui::End();
}
