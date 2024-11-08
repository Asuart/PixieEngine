#pragma once
#include "pch.h"
#include "PixieEngineInterfaceWindow.h"

class AssetsBrowserWindow: public PixieEngineInterfaceWindow {
public:
	AssetsBrowserWindow(PixieEngineApp& app, PixieEngineInterface& inter);

	void Initialize() override;
	void Draw() override;

protected:
	float m_thumbnailSize = 96.0f;
	float m_thumbnailPadding = 12.0f;
	GLuint m_fileIcon = 0;
	GLuint m_folderIcon = 0;
	GLuint m_parentFolderIcon = 0;
};
