#include "pch.h"
#include "PixieEngineEditor.h"
#include "PixieEngineCore.h"

int32_t main(int32_t argc, char** argv) {	
	PixieEngineMainWindow* mainWindow = PixieEngine::Initialize(argv[0]);
	if (!mainWindow) {
		Log::Error("Failed to initialize Pixie Engine");
		return 1;
	}

	PixieEngineEditor app(mainWindow);
	app.Initialize();
	app.Start();

	PixieEngine::Free();

	return 0;
}
