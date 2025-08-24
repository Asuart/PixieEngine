#pragma once
#include "pch.h"
#include "MainWindow.h"

namespace PixieEngine {

class MainWindowVulkan : public MainWindow {
public:
	MainWindowVulkan(const std::string& name, glm::ivec2 resolution);
	~MainWindowVulkan();

protected:
	uint32_t m_imageIndex;

	void InitSDL();
};

}