#include "pch.h"
#include "PixieEngineApp.h"

PixieEngineApp::PixieEngineApp()
	: m_interface(*this) {
	if (!gladLoadGL()) {
		std::cout << "GLEW initialization failed.\n";
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window.GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init();

	SceneLoader sceneLoader;
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_scene->MakeGeometrySnapshot();

	m_sceneRenderer = new SceneRenderer(glm::ivec2(640, 640), m_scene);

	m_rayTracingRenderer = new RayTracingRenderer(this, glm::ivec2(640, 640), m_scene);
	if (m_rayTracingViewport) {
		m_rayTracingRenderer->StartRender();
	}

	m_viewportFrameBuffer = new FrameBuffer(640, 640);
}

PixieEngineApp::~PixieEngineApp() {
	delete m_rayTracingRenderer;
	delete m_viewportFrameBuffer;
}

void PixieEngineApp::Start() {
	while (!m_window.IsShouldClose()) {
		Timer::Update();
		glfwPollEvents();
		HandleUserInput();
		UserInput::Reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_viewportFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
		if (m_rayTracingViewport) {
			if (m_rayTracingRenderer->m_rayTracer->GetSamplesCount() == 0) {
				m_sceneRenderer->DrawFrame();
			}
			m_rayTracingRenderer->DrawFrame();
		}
		else {
			m_sceneRenderer->DrawFrame();
		}
		m_viewportFrameBuffer->Unbind();
		glViewport(0, 0, m_window.GetWindowSize().x, m_window.GetWindowSize().y);

		m_interface.Draw();

		glfwSwapBuffers(m_window.GetGLFWWindow());
	}

	m_rayTracingRenderer->StopRender();
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_window.GetGLFWWindow();
}

void PixieEngineApp::HandleResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_rayTracingRenderer->SetViewportSize(resolution);
	if (m_rayTracingRenderer->m_resizeRendererToVieport) {
		m_sceneRenderer->SetResolution(resolution);
		m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
	}
}

void PixieEngineApp::ReloadScene() {
	m_rayTracingRenderer->StopRender();
	if (m_scene) {
		delete m_scene;
		m_scene = nullptr;
	}

	SceneLoader sceneLoader;
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_scene->MakeGeometrySnapshot();

	m_sceneRenderer->SetScene(m_scene);

	m_rayTracingRenderer->SetScene(m_scene);
	m_rayTracingRenderer->StartRender();
}

void PixieEngineApp::HandleUserInput() {
	const Float speed = 10.0f;
	const Float rotationSpeed = 50.0f;

	if (UserInput::GetKey(GLFW_KEY_W)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveForward(speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport) m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_S)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveForward(-speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_D)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveRight(speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_A)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveRight(-speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_SPACE)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveUp(speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetKey(GLFW_KEY_LEFT_CONTROL)) {
		if (m_scene) {
			Camera* camera = m_scene->GetMainCamera();
			if (camera) {
				camera->m_transform.MoveUp(-speed * Timer::deltaTime);
			}
		}
		if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
	}
	if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_2)) {
		if (UserInput::mouseDeltaX) {
			if (UserInput::mouseDeltaX) {
				if (m_scene) {
					Camera* camera = m_scene->GetMainCamera();
					if (camera) {
						camera->m_transform.AddRotationY(-rotationSpeed * (Float)UserInput::mouseDeltaX * Timer::deltaTime);
					}
				}
			}
			if (m_rayTracingViewport)m_rayTracingRenderer->Reset();
		}
	}
}