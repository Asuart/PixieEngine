#include "UI.h"

namespace PixieEngine {

UI::UI(MainWindow* window, bool docking) :
	m_window(window), m_isDocking(docking) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui_ImplSDL2_InitForOpenGL(window->GetSDLWindow(), window->GetOpenGLContext());
		ImGui_ImplOpenGL3_Init();
	}
	else if(MainWindowVulkan* window = dynamic_cast<MainWindowVulkan*>(m_window)) {
		ImGui_ImplSDL2_InitForVulkan(window->GetSDLWindow());
		//ImGui_ImplVulkan_Init();
	}
}

UI::~UI() {
	for (size_t i = 0; i < m_windows.size(); i++) {
		delete m_windows[i];
	}
	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}
	else if (MainWindowVulkan* window = dynamic_cast<MainWindowVulkan*>(m_window)) {

	}
	ImGui::DestroyContext();
}

void UI::HandleEvent(const Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event.sdlEvent);
}

void UI::AddWindow(UIWindow* window) {
	if (!window) {
		return;
	}
	m_windows.push_back(window);
}

void UI::Draw() {
	if (MainWindowOpenGL* window = dynamic_cast<MainWindowOpenGL*>(m_window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (m_isDocking) {
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &m_isDocking, windowFlags);

		ImGui::PopStyleVar(3);

		if (m_isDocking) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		for (size_t i = 0; i < m_windows.size(); i++) {
			m_windows[i]->Draw();
		}

		ImGui::End();

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(window->GetSDLWindow(), window->GetOpenGLContext());
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

}