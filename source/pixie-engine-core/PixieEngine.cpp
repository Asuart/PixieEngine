#include "pch.h"
#include "PixieEngineCore.h"

PixieEngineMainWindow* PixieEngine::Initialize(const std::string& rootPath) {
	Globals::SetApplicationPath(rootPath);
	PixieEngineMainWindow* window = new PixieEngineMainWindow("PixieEngine Main Window", { 1280, 720 });
	if (!RenderEngine::Initialize()) {
		Log::Error("Failed to initialize Render Engine");
		delete window;
		return nullptr;
	}
	return window;
}

void PixieEngine::Free() {
	RenderEngine::Free();
}
