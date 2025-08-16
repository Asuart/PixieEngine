#pragma once
#include "pch.h"
#include "Rendering/RenderEngine.h"
#include "Windowing/MainWindowOpenGL.h"
#include "Windowing/MainWindowVulkan.h"

namespace PixieEngine {

bool Initialize(const std::string& rootPath);
void Free();

MainWindow* CreateMainWindow(const std::string& windowName, RenderAPI renderAPI);

}