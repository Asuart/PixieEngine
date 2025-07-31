#include "pch.h"
#include "Rendering/RenderEngine.h"
#include "PixieEngineMainWindow.h"
#include "Log.h"

class PixieEngine {
public:
	PixieEngine() = delete;

	static PixieEngineMainWindow* Initialize(const std::string& rootPath);
	static void Free();
};
