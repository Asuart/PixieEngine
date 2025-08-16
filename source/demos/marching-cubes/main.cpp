#include "PixieEngineCore.h"
#include "UI.h"
#include "Resources/MarchingCubes.h"


int32_t main(int32_t argc, char** argv) {
	PixieEngineMainWindow* mainWindow = PixieEngine::Initialize(argv[0]);
	if (!mainWindow) {
		Log::Error("Failed to initialize Pixie Engine");
		return 1;
	}

	UI ui(mainWindow->GetSDLWindow(), mainWindow->GetGLContext());

	MarchingCubesTerrain terrain = MarchingCubesTerrain();

	while (!mainWindow->ShouldClose()) {
		ui.Draw();
		SDL_GL_SwapWindow(mainWindow->GetSDLWindow());

		for (int32_t x = 0; x < MarchingCubesTerrain::cSize; x++) {
			for (int32_t y = 0; y < MarchingCubesTerrain::cSize; y++) {
				for (int32_t z = 0; z < MarchingCubesTerrain::cSize; z++) {
					RenderEngine::DrawMesh(terrain.GetChunk({ x, y, z }).mesh);
				}
			}
		}

		SDL_Event evnt;
		while (SDL_PollEvent(&evnt)) {
			ImGui_ImplSDL2_ProcessEvent(&evnt);
			if (evnt.type == SDL_QUIT) {
				mainWindow->Close();
			}
			if (evnt.type == SDL_WINDOWEVENT && evnt.window.event == SDL_WINDOWEVENT_CLOSE && evnt.window.windowID == SDL_GetWindowID(mainWindow->GetSDLWindow())) {
				mainWindow->Close();
			}
		}
	}

    return 0;
}
