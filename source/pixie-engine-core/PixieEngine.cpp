#include "pch.h"
#include "PixieEngineCore.h"
#include "Debug/Log.h"

namespace PixieEngine {

bool PixieEngine::Initialize(const std::string& rootPath) {
	EngineConfig::SetApplicationPath(rootPath);
	return true;
}

MainWindow* PixieEngine::CreateMainWindow(const std::string& windowName, RenderAPI renderAPI) {
	RenderAPI currentRenderAPI = RenderEngine::GetRenderAPI();
	if (currentRenderAPI != RenderAPI::Undefined) {
		// TODO: reload resources to support new render API
	}

	if (renderAPI == RenderAPI::OpenGL) {
		MainWindow* window = new MainWindowOpenGL(windowName, { 1280, 720 });
		if (!RenderEngine::Initialize(renderAPI)) {
			Log::Error("Failed to initialize Render Engine");
			delete window;
			return nullptr;
		}
		return window;
	}
	else if (renderAPI == RenderAPI::Vulkan) {
		MainWindow* window = new MainWindowVulkan(windowName, { 1280, 720 });
		if (!RenderEngine::Initialize(renderAPI)) {
			Log::Error("Failed to initialize Render Engine");
			delete window;
			return nullptr;
		}
		return window;
	}
	Log::Error("Unhandled Render API");
	return nullptr;
}

void PixieEngine::Free() {
	RenderEngine::Free();
}

}