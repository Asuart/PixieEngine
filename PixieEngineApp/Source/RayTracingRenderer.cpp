#include "pch.h"
#include "RayTracingRenderer.h"

std::string to_string(RayTracingVisualization mode) {
	switch (mode) {
	case RayTracingVisualization::Integration: return "Integration";
	case RayTracingVisualization::BoxChecksStatistics: return "Box Checks Statistics";
	case RayTracingVisualization::TriangleChecksStatistics: return "Triangle Checks Statistics";
	default: return "Undefined Visualization Mode";
	}
}

std::string to_string(RayTracingMode mode) {
	switch (mode) {
	case RayTracingMode::RandomWalk: return "Random Walk";
	case RayTracingMode::SimplePathTracing: return "Simple Path Tracing";
	case RayTracingMode::PathTracing: return "Path Tracing";
	case RayTracingMode::TestNormals: return "Test Normals";
	case RayTracingMode::TestSampler: return "Test Sampler";
	default: return "Undefined Ray Tracing Mode";
	}
}

RayTracingRenderer::RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, Scene* scene)
	: m_parent(parent), m_resolution(resolution), m_rayTracer(new PathIntegrator(resolution)), m_scene(scene), m_viewportResolution(resolution) {
	SetScene(scene);
}

RayTracingRenderer::~RayTracingRenderer() {
	delete m_rayTracer;
}

void RayTracingRenderer::DrawFrame() {
	GLuint program = m_rayTracer->m_film.mesh->shader;
	GLuint posLoc = glGetUniformLocation(program, "uPos");
	GLuint sizeLoc = glGetUniformLocation(program, "uSize");
	GLuint samplesLoc = glGetUniformLocation(program, "uSamples");

	float textureAspect = (float)m_resolution.x / m_resolution.y;
	float viewportAspect = (float)m_viewportResolution.x / m_viewportResolution.y;
	float posX, posY;
	float sizeX, sizeY;
	if (viewportAspect > textureAspect) {
		sizeY = 1.0f;
		sizeX = textureAspect / viewportAspect;
		posX = (1.0f - sizeX) * 0.5f;
		posY = 0.0f;
	}
	else {
		sizeX = 1.0f;
		sizeY = viewportAspect / textureAspect;
		posX = 0.0f;
		posY = (1.0f - sizeY) * 0.5f;
	}

	glUseProgram(program);
	glUniform2f(posLoc, posX, posY);
	glUniform2f(sizeLoc, sizeX, sizeY);

	glActiveTexture(GL_TEXTURE0);
	switch (m_visualizationMode) {
	case RayTracingVisualization::Integration:
		m_rayTracer->m_film.texture->Upload();
		glUniform1f(samplesLoc, (float)m_rayTracer->GetSamplesCount());
		m_rayTracer->m_film.texture->Bind(GL_TEXTURE0);
		break;
	case RayTracingVisualization::BoxChecksStatistics:
		m_rayTracer->m_stats.UploadBoxTestsTextureLinear();
		glUniform1f(samplesLoc, 1.0f);
		m_rayTracer->m_stats.BindBoxTestsTexture();
		break;
	case RayTracingVisualization::TriangleChecksStatistics:
		m_rayTracer->m_stats.UploadTriangleTestsTextureLinear();
		glUniform1f(samplesLoc, 1.0f);
		m_rayTracer->m_stats.BindTriangleTestsTexture();
		break;
	}

	m_rayTracer->m_film.mesh->Draw();
}

void RayTracingRenderer::Reset() {
	m_rayTracer->Reset();
}

void RayTracingRenderer::SetScene(Scene* scene) {
	m_scene = scene;
	m_rayTracer->SetScene(scene);
	for (Camera& camera : m_scene->GetCameras()) {
		camera.SetAspect((float)m_resolution.x / m_resolution.y);
	}
}

void RayTracingRenderer::StartRender() {
	m_rayTracer->StartRender();
}

void RayTracingRenderer::StopRender() {
	m_rayTracer->StopRender();
}

void RayTracingRenderer::DrawUI() {
	ImGui::Text("Visualization Mode");
	if (ImGui::BeginCombo("##visualization_mode", to_string(m_visualizationMode).c_str())) {
		for (int32_t n = 0; n < (int32_t)RayTracingVisualization::COUNT; n++) {
			RayTracingVisualization mode = RayTracingVisualization(n);
			bool isSelected = (m_visualizationMode == mode);
			if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
				SetVisualizationMode(mode);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	ImGui::Text("Ray Tracing Mode");
	if (ImGui::BeginCombo("##ray_tracing_mode", to_string(m_rayTracingMode).c_str())) {
		for (int32_t n = 0; n < (int32_t)RayTracingMode::COUNT; n++) {
			RayTracingMode mode = RayTracingMode(n);
			bool isSelected = (m_rayTracingMode == mode);
			if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
				SetRayTracingMode(mode);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	ImGui::Text("Render Resolution");
	if (ImGui::InputInt2("##render_resolution", (int*)&m_resolution)) {
		m_rayTracer->SetResolution(m_resolution);
		for (Camera& camera : m_scene->GetCameras()) {
			camera.SetAspect((float)m_resolution.x / m_resolution.y);
		}
	}
	if (ImGui::Checkbox("Resize to viewport", &m_resizeRendererToVieport)) {
		Reset();
		SetViewportSize(m_viewportResolution);
	}
	ImGui::Spacing();

	ImGui::Text("Max Ray Bounces");
	if (ImGui::InputInt("##max_ray_bounces", &m_rayTracer->m_maxDepth)) {
		m_rayTracer->m_maxDepth = Clamp(m_rayTracer->m_maxDepth, 0, 99999);
		Reset();
	}
	ImGui::Spacing();

	std::string activeCameraName = "Camera ";
	const std::vector<Camera>& cameras = m_scene->GetCameras();
	for (size_t i = 0; i < cameras.size(); i++) {
		if (m_scene->GetMainCamera() == &cameras[i]) {
			activeCameraName += std::to_string(i);
			break;
		}
	}
	ImGui::Text("Active Camera");
	if (ImGui::BeginCombo("##active_camera", activeCameraName.c_str())) {
		for (size_t i = 0; i < cameras.size(); i++) {
			bool isSelected = m_scene->GetMainCamera() == &cameras[i];
			if (ImGui::Selectable(("Camera " + std::to_string(i)).c_str(), isSelected)) {
				m_scene->SetMainCamera((uint32_t)i);
				m_rayTracer->Reset();
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	ImGui::Text("Max Render Threads");
	if (ImGui::InputInt("##max_render_threads", &m_rayTracer->m_maxThreads)) {
		m_rayTracer->m_maxThreads = Clamp(m_rayTracer->m_maxThreads, 1, 128);
		m_rayTracer->Reset();
	}
	ImGui::Spacing();

	if (m_rayTracingMode == RayTracingMode::SimplePathTracing) {
		SimplePathIntegrator* integrator = dynamic_cast<SimplePathIntegrator*>(m_rayTracer);
		if (ImGui::Checkbox("Sample Lights", &integrator->m_sampleLights)) {
			Reset();
		}
		if (ImGui::Checkbox("Sample BSDF", &integrator->m_sampleBSDF)) {
			Reset();
		}
		ImGui::Spacing();
	}
	else if (m_rayTracingMode == RayTracingMode::PathTracing) {
		PathIntegrator* integrator = dynamic_cast<PathIntegrator*>(m_rayTracer);
		if (ImGui::Checkbox("Regularize BSDF", &integrator->m_regularize)) {
			Reset();
		}
		ImGui::Spacing();
	}

	std::string threadsText = std::string("Threads: ") + std::to_string(m_rayTracer->GetThreadsCount());
	ImGui::Text(threadsText.c_str());

	std::string samplesText = std::string("Samples: ") + std::to_string(m_rayTracer->GetSamplesCount());
	ImGui::Text(samplesText.c_str());

	std::string renderTimeText = std::string("Render Time: ") + std::to_string(m_rayTracer->GetRenderTime());
	ImGui::Text(renderTimeText.c_str());

	std::string lastSampleTimeText = std::string("Last Sample Time: ") + std::to_string(m_rayTracer->GetLastSampleTime());
	ImGui::Text(lastSampleTimeText.c_str());

	uint64_t totalRays = m_rayTracer->m_stats.GetTotalRays();
	std::string totalRaysText = std::string("Rays: ") + std::to_string(totalRays);
	ImGui::Text(totalRaysText.c_str());

	uint64_t totalBoxes = m_rayTracer->m_stats.GetTotalBoxTests();
	std::string totalBoxesText = std::string("Box checks: ") + std::to_string(totalBoxes);
	ImGui::Text(totalBoxesText.c_str());

	uint64_t totalTriangles = m_rayTracer->m_stats.GetTotalTriangleTests();
	std::string totalTrianglesText = std::string("Triangle checks: ") + std::to_string(totalTriangles);
	ImGui::Text(totalTrianglesText.c_str());
}

void RayTracingRenderer::SetViewportSize(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	if (m_resizeRendererToVieport) {
		m_resolution = resolution;
		m_rayTracer->SetResolution(resolution);
		std::vector<Camera>& cameras = m_scene->GetCameras();
		for (size_t i = 0; i < cameras.size(); i++) {
			cameras[i].SetAspect((float)resolution.x / resolution.y);
		}
	}
}

void RayTracingRenderer::SetVisualizationMode(RayTracingVisualization mode) {
	m_visualizationMode = mode;
}

void RayTracingRenderer::SetRayTracingMode(RayTracingMode mode) {
	if (mode == m_rayTracingMode) return;
	m_rayTracingMode = mode;

	if (m_rayTracer) {
		m_rayTracer->StopRender();
		delete m_rayTracer;
	}

	switch (m_rayTracingMode) {
	case RayTracingMode::SimplePathTracing: m_rayTracer = new SimplePathIntegrator(m_resolution); break;
	case RayTracingMode::PathTracing: m_rayTracer = new PathIntegrator(m_resolution); break;
	case RayTracingMode::TestNormals: m_rayTracer = new TestNormalsIntegrator(m_resolution); break;
	case RayTracingMode::TestSampler: m_rayTracer = new TestSamplerIntegrator(m_resolution); break;
	case RayTracingMode::RandomWalk: default:
		m_rayTracer = new RandomWalkIntegrator(m_resolution); break;
	}

	m_rayTracer->SetScene(m_scene);
	m_rayTracer->StartRender();
}
