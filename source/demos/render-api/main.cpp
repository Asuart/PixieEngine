#include <functional>

#include <glm/glm.hpp>

#include "PixieEngineCore.h"
#include "UI.h"
#include "DemoWindow.h"

using namespace PixieEngine;

class RenderAPISelect : public PixieEngine::UIWindow {
public:
	RenderAPISelect(std::function<void(PixieEngine::RenderAPI)> selectCallback) :
		m_selectCallback(selectCallback) {}

	void Draw() override {
		ImGui::SetNextWindowSize(ImVec2(400, 400));
		if (ImGui::Begin("Render API Select", 0)) {
			ImGui::Text("Select a render API at runtime:");
			
			PixieEngine::RenderAPI activeRenderMode = PixieEngine::RenderEngine::GetRenderAPI();
			if (ImGui::BeginCombo("Render Mode", to_string(activeRenderMode).data())) {
				for (int32_t n = 1; n < (int32_t)PixieEngine::RenderAPI::COUNT; n++) {
					PixieEngine::RenderAPI mode = PixieEngine::RenderAPI(n);
					bool isSelected = (activeRenderMode == mode);
					if (ImGui::Selectable(to_string(mode).data(), isSelected)) {
						m_selectCallback(mode);
					}
					if (isSelected) {
						activeRenderMode = mode;
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

		}
		ImGui::End();
	}

protected:
	std::function<void(PixieEngine::RenderAPI)> m_selectCallback;
};

class PixieEngineApplication {
public:
	void Start() {
		m_switchingRenderAPI = true;
		auto renderAPISelectCallback = [&](PixieEngine::RenderAPI api) {
			m_renderAPI = api;
			m_switchingRenderAPI = true;
			m_mainWindow->Close();
			};

		while (m_switchingRenderAPI) {
			m_switchingRenderAPI = false;

			PixieEngine::RenderAPI api = PixieEngine::RenderEngine::GetRenderAPI();
			m_mainWindow = PixieEngine::CreateMainWindow("Render API Demo", m_renderAPI);
			if (!m_mainWindow) {
				PixieEngine::Log::Error("Failed to Create Main Window");
				exit(1);
			}

			m_ui = new PixieEngine::UI(m_mainWindow, false);
			m_ui->AddWindow(new PixieEngine::DemoWindow());
			m_ui->AddWindow(new RenderAPISelect(renderAPISelectCallback));

			MainLoop();

			delete m_ui;
			delete m_mainWindow;
			PixieEngine::Free();
		}
	}

protected:
	PixieEngine::RenderAPI m_renderAPI = PixieEngine::RenderAPI::Vulkan;
	bool m_switchingRenderAPI = true;
	PixieEngine::MainWindow* m_mainWindow = nullptr;
	PixieEngine::UI* m_ui = nullptr;

	void MainLoop() {
		while (!m_mainWindow->ShouldClose()) {
			m_mainWindow->Clear();
			m_ui->Draw();
			m_mainWindow->SwapBuffers();

			while (std::optional<PixieEngine::Event> event = m_mainWindow->PollEvent()) {
				m_ui->HandleEvent(event.value());
				m_mainWindow->HandleEvent(event.value());
			}
		}
	}
};

int32_t main(int32_t argc, char** argv) {
	PixieEngine::Initialize(argv[0]);

	PixieEngineApplication app;
	app.Start();

    return 0;
}
