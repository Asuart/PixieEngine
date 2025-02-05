#include "pch.h"
#include "StatsWindow.h"
#include "ViewportWindow.h"
#include "Interface.h"

StatsWindow::StatsWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void StatsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Stats", 0)) {
		std::vector<ViewportWindow*> RTViewports = m_interface.GetWindowsOfType<ViewportWindow>();
		int32_t totalThreads = 1;
		for (int32_t i = 0; i < RTViewports.size(); i++) {
			//totalThreads += RTViewports[i]->GetThreadsCount();
		}

		std::string threadsText = std::string("Threads: ") + std::to_string(totalThreads);
		ImGui::Text(threadsText.c_str());
	}
	ImGui::End();
}