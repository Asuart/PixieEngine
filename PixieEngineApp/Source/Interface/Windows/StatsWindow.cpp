#include "pch.h"
#include "StatsWindow.h"
#include "ViewportWindow.h"
#include "Interface.h"


StatsWindow::StatsWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void StatsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Stats", 0)) {
		for (size_t i = 0; i < HighPrecisionTimer::s_timers.size(); i++) {
			double milli = (double)HighPrecisionTimer::s_timers[i].m_lastDelta.count() / 1000000.0f;
			ImGui::Text((HighPrecisionTimer::s_timers[i].m_name + std::string(": ") + std::to_string(milli)).c_str());
		}
	}
	ImGui::End();
}
