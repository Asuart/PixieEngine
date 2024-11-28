#include "pch.h"
#include "RayTracingSettingsWindow.h"
#include "PixieEngineInterface.h"

RayTracingSettingsWindow::RayTracingSettingsWindow(PixieEngineApp& app, PixieEngineInterface& inter) :
	PixieEngineInterfaceWindow(app, inter) {}

void RayTracingSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("RT Settings", 0)) {
		std::vector<RayTracingViewportWindow*> viewports = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
		if (viewports.size() > 0) {
			RayTracingViewportWindow* viewport = viewports[0];

			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();

			ImGui::PushItemWidth(-FLT_MIN);

			ImGui::Text("Ray Tracing Mode");
			if (ImGui::BeginCombo("##ray_tracing_mode", to_string(viewport->m_rayTracingMode).c_str())) {
				for (int32_t n = 0; n < (int32_t)RayTracingMode::COUNT; n++) {
					RayTracingMode mode = RayTracingMode(n);
					bool isSelected = (viewport->m_rayTracingMode == mode);
					if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
						viewport->SetRayTracingMode(mode);
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

			ImGui::Text("Visualization Mode");
			if (ImGui::BeginCombo("##visualization_mode", to_string(viewport->m_visualizationMode).c_str())) {
				for (int32_t n = 0; n < (int32_t)RayTracingVisualization::COUNT; n++) {
					RayTracingVisualization mode = RayTracingVisualization(n);
					bool isSelected = (viewport->m_visualizationMode == mode);
					if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
						viewport->SetVisualizationMode(mode);
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

			ImGui::Text("Max Render Threads");
			if (ImGui::InputInt("##max_render_threads", &viewport->m_maxThreads)) {
				viewport->m_maxThreads = Clamp(viewport->m_maxThreads, 1, 128);
				viewport->Reset();
			}
			ImGui::Spacing();

			std::string samplesText = std::string("Samples: ") + std::to_string(viewport->GetSamplesCount());
			ImGui::Text(samplesText.c_str());

			std::string renderTimeText = std::string("Render Time: ") + std::to_string(viewport->GetRenderTime());
			ImGui::Text(renderTimeText.c_str());

			std::string lastSampleTimeText = std::string("Last Sample Time: ") + std::to_string(viewport->GetLastSampleTime());
			ImGui::Text(lastSampleTimeText.c_str());

			ImGui::PopItemWidth();
		}
	}
	ImGui::End();
}
