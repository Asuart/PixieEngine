#include "pch.h"
#include "MainWindowMenu.h"
#include "PixieEngineApp.h"
#include "Interface.h"
#include "SceneGenerator.h"

MainWindowMenu::MainWindowMenu(PixieEngineApp& app, Interface& inter) :
	m_app(app), m_interface(inter) {}

void MainWindowMenu::Draw() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Close")) {
				glfwSetWindowShouldClose(m_app.GetGLFWWindow(), true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Test Scenes")) {
			if (ImGui::MenuItem("Empty Scene")) {
				SceneGenerator::CreateScene(GeneratedScene::Empty);
			}
			if (ImGui::MenuItem("Demo Sphere")) {
				SceneGenerator::CreateScene(GeneratedScene::DemoSphere);
			}
			if (ImGui::MenuItem("Small And Big Spheres")) {
				SceneGenerator::CreateScene(GeneratedScene::SmallAndBigSpheres);
			}
			if (ImGui::MenuItem("Test Materials")) {
				SceneGenerator::CreateScene(GeneratedScene::TestMaterials);
			}
			if (ImGui::MenuItem("Randomized Spheres")) {
				SceneGenerator::CreateScene(GeneratedScene::RandomizedSpheres);
			}
			if (ImGui::MenuItem("Spheres Array")) {
				SceneGenerator::CreateScene(GeneratedScene::SpheresArray);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create Object")) {
			if (ImGui::MenuItem("Sphere")) {
				SceneGenerator::CreateObject(GeneratedObject::Sphere);
			}
			if (ImGui::MenuItem("Point Light")) {
				SceneGenerator::CreateObject(GeneratedObject::PointLight);
			}
			if (ImGui::MenuItem("Directional Light")) {
				SceneGenerator::CreateObject(GeneratedObject::DirectionalLight);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}
