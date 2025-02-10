#include "pch.h"
#include "PathTracingRenderer.h"

PathTracingRenderer::PathTracingRenderer() :
	m_frameBuffer({1280, 720}), m_camera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 10),
	m_film({ 1280, 720 }), m_boxTestsTexture({ 1280, 720 }), m_shapeTestsTexture({ 1280, 720 }),
	m_normalTexture({ 1280, 720 }), m_depthTexture({ 1280, 720 }) {}

PathTracingRenderer::~PathTracingRenderer() {
	StopRender();
}

void PathTracingRenderer::DrawFrame() {
	GLint originalViewport[4];
	glGetIntegerv(GL_VIEWPORT, originalViewport);

	m_frameBuffer.Bind();
	m_frameBuffer.ResizeViewport();
	m_frameBuffer.Clear();
	m_film.m_texture.Upload();
	GlobalRenderer::DrawAccumulatorTextureFitted(m_film.m_texture.GetID(), m_samples, m_film.m_texture.GetResolution(), m_frameBuffer.GetResolution());
	m_frameBuffer.Unbind();

	glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
}

void PathTracingRenderer::Reset() {
	bool wasRendering = m_isRendering;
	StopRender();
	m_film.Reset();
	m_samples = 1;
	if (wasRendering) StartRender(m_sceneSnapshot, m_camera);
}

void PathTracingRenderer::StartRender(std::shared_ptr<SceneSnapshot> sceneSnapshot, const Camera& camera) {
	if (!sceneSnapshot) return;
	if (m_isRendering) return;
	m_sceneSnapshot = sceneSnapshot;
	m_camera = camera;
	m_frameBuffer.Resize(m_camera.GetResolution());
	m_film.Resize(m_camera.GetResolution());
	m_samples = 1;
	m_isRendering = true;

	m_renderStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	m_sampleStartTime = m_renderStartTime;

	m_rayTracer.SetSceneSnapshot(m_sceneSnapshot.get());

	GenerateTiles();
	ResetTileQueue();

	m_threadsCount = std::min(m_maxThreads, (int32_t)m_tiles.size());

	for (int32_t i = 0; i < m_threadsCount; i++) {
		m_renderThreads.push_back(new std::thread([&]() {
			std::shared_ptr<Sampler> sampler = std::make_shared<IndependentSampler>(m_samplesPerPixel);
			while (m_isRendering && m_samples < m_samplesPerPixel) {
				m_tileQueueMutex.lock();
				if (m_tileQueue.empty()) {
					std::chrono::microseconds currrentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
					m_lastSampleTime = currrentTime - m_sampleStartTime;
					m_sampleStartTime = currrentTime;
					m_samples++;
					for (size_t i = 0; i < m_tiles.size(); i++) {
						m_tileQueue.push((int32_t)i);
					}
				}
				int32_t index = m_tileQueue.front();
				m_tileQueue.pop();
				m_tileQueueMutex.unlock();

				Bounds2i quad = m_tiles[index];
				for (int32_t y = quad.min.y; y < quad.max.y; y++) {
					for (int32_t x = quad.min.x; x < quad.max.x; x++) {
						sampler->StartPixelSample(glm::ivec2(x, y), m_samples);
						PerPixel(x, y, sampler.get());
						if (!m_isRendering) return;
					}
				}
			}
			})
		);
	}
}

void PathTracingRenderer::PauseRender() {
	// TODO
	std::cout << "Ray tracing pause is not implemented.\n";
}

void PathTracingRenderer::StopRender() {
	if (!m_isRendering) return;
	m_isRendering = false;
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		m_renderThreads[i]->join();
	}
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		delete m_renderThreads[i];
	}
	m_renderThreads.clear();
}

uint32_t PathTracingRenderer::GetSamplesCount() const {
	return m_samples;
}

int32_t PathTracingRenderer::GetMaxRenderThreads() {
	return m_maxThreads;
}

void PathTracingRenderer::SetMaxRenderThreads(int32_t count) {
	if (m_maxThreads == count) return;
	bool wasRendering = m_isRendering;
	StopRender();
	m_maxThreads = count;
	if (wasRendering) StartRender(m_sceneSnapshot, m_camera);
}

Float PathTracingRenderer::GetRenderTime() const {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return (newTime - m_renderStartTime).count() / 1000.0f;
}

Float PathTracingRenderer::GetLastSampleTime() const {
	return m_lastSampleTime.count() / 1000.0f;
}

void PathTracingRenderer::PerPixel(uint32_t x, uint32_t y, Sampler* sampler) {
	FilmFilter* filter = m_film.GetFilter();
	CameraSample cameraSample = GetCameraSample(x, y, filter, sampler);
	Vec2 uv = m_film.GetUV(cameraSample.pFilm);
	Ray ray = m_camera.GetRay(uv);
	GBufferPixel pixel = m_rayTracer.SampleLightRay(ray, sampler);
	m_boxTestsTexture.SetPixel({ x, y }, (Float)pixel.boxChecks);
	m_shapeTestsTexture.SetPixel({ x, y }, (Float)pixel.shapeChecks);
	m_normalTexture.SetPixel({ x, y }, glm::abs(pixel.normal));
	m_depthTexture.SetPixel({ x, y }, pixel.depth);
	m_film.AddSample(x, y, pixel.light, cameraSample.filterWeight);
}

CameraSample PathTracingRenderer::GetCameraSample(uint32_t x, uint32_t y, const FilmFilter* filter, Sampler* sampler) {
	FilmFilterSample fs = filter->Sample(sampler->GetPixel2D());
	return CameraSample(Vec2(x, y) + fs.p + Vec2(0.5f, 0.5f), fs.weight);
}

void PathTracingRenderer::GenerateTiles() {
	m_tiles.clear();
	glm::ivec2 resolution = m_film.m_resolution;
	glm::ivec2 tileGridSize = glm::ceil(Vec2(resolution) / Vec2(m_tileSize));
	for (int32_t yTile = 0; yTile < tileGridSize.y; yTile++) {
		for (int32_t xTile = 0; xTile < tileGridSize.x; xTile++) {
			int32_t xMax = glm::min(resolution.x, (xTile + 1) * m_tileSize.x);
			int32_t yMax = glm::min(resolution.y, (yTile + 1) * m_tileSize.y);
			glm::ivec2 min = glm::ivec2(xTile * m_tileSize.x, yTile * m_tileSize.y);
			glm::ivec2 max = glm::ivec2(xMax, yMax);
			m_tiles.push_back(Bounds2i(min, max));
		}
	}
}

void PathTracingRenderer::ResetTileQueue() {
	m_tileQueue = std::queue<int32_t>();
	for (size_t i = 0; i < m_tiles.size(); i++) {
		m_tileQueue.push((int32_t)i);
	}
}
