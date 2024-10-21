#include "pch.h"
#include "Integrator.h"

void clear(std::queue<int32_t>& q) {
	std::queue<int32_t> empty;
	std::swap(q, empty);
}

Integrator::Integrator(const glm::ivec2& resolution)
	: Renderer(resolution), m_film(Film(resolution)) {
	RayTracingStatistics::Resize(resolution);
}

void Integrator::SetScene(Scene* scene) {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}
	m_scene = scene;
	Reset();
	if (wasRendering) {
		StartRender();
	}
}

void Integrator::SetResolution(const glm::ivec2& resolution) {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}
	m_resolution = resolution;
	m_film.Resize(resolution);
	RayTracingStatistics::Resize(resolution);
	Reset();
	if (wasRendering) {
		StartRender();
	}
}

void Integrator::Reset() {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}

	m_film.Reset();
	RayTracingStatistics::Reset();
	GenerateTiles();
	m_samples = 1;

	Bounds3f sceneBounds = m_scene->GetBounds();
	const std::vector<DiffuseAreaLight*>& areaLights = m_scene->GetGeometrySnapshot()->GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		areaLights[i]->Preprocess(sceneBounds);
	}
	const std::vector<Light*>& lights = m_scene->GetInfiniteLights();
	for (size_t i = 0; i < lights.size(); i++) {
		lights[i]->Preprocess(sceneBounds);
	}

	if (wasRendering) {
		StartRender();
	}
}

void Integrator::StartRender() {
	if (!m_scene || m_isRendering) return;
	m_isRendering = true;

	m_renderStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	m_sampleStartTime = m_renderStartTime;

	clear(m_tileQueue);
	for (size_t i = 0; i < m_tiles.size(); i++) {
		m_tileQueue.push((int32_t)i);
	}

	m_threadsCount = std::min(m_maxThreads, (int32_t)m_tiles.size());

	m_spp = 32768, m_waveStart = 0, m_waveEnd = 1, m_nextWaveSize = 1;
	for (int32_t i = 0; i < m_threadsCount; i++) {
		m_renderThreads.push_back(new std::thread([&]() {
			Sampler* sampler = new IndependentSampler(m_spp);
			while (m_isRendering && m_waveStart < m_spp) {
				m_tileQueueMutex.lock();
				if (m_tileQueue.empty()) {
					std::chrono::microseconds currrentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
					m_lastSampleTime = currrentTime - m_sampleStartTime;
					m_sampleStartTime = currrentTime;

					m_samples += m_nextWaveSize;
					m_waveStart = m_waveEnd;
					m_waveEnd = std::min(m_spp, m_waveEnd + m_nextWaveSize);
					m_nextWaveSize = std::min(2 * m_nextWaveSize, m_maxWaveSize);

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
						for (int32_t sampleIndex = m_waveStart; sampleIndex < m_waveEnd; sampleIndex++) {
							sampler->StartPixelSample(glm::ivec2(x, y), sampleIndex);
							PerPixel(x, y, sampler);
						}
					}
				}
			}
			delete sampler;
			}));
	}
}

void Integrator::StopRender() {
	if (!m_isRendering) return;
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		m_renderThreads[i]->join();
	}
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		delete m_renderThreads[i];
	}
	m_renderThreads.clear();
	m_isRendering = false;
}

void Integrator::PerPixel(uint32_t x, uint32_t y, Sampler* sampler) {
	g_threadPixelCoordX = x;
	g_threadPixelCoordY = y;
	RayTracingStatistics::IncrementPixelSamples();

	SampleFilter* filter = m_film.GetFilter();
	CameraSample cameraSample = GetCameraSample(x, y, filter, sampler);
	Vec2 uv = m_film.GetUV(cameraSample.pFilm);
	Ray ray = m_scene->GetMainCamera()->GetRay(uv);
	Spectrum L = Integrate(ray, sampler);
	m_film.AddSample(x, y, L, cameraSample.filterWeight);
}

bool Integrator::Unoccluded(const SurfaceInteraction& p0, const SurfaceInteraction& p1) {
	RayTracingStatistics::IncrementRays();
	Vec3 dir = p1.position - p0.position;
	Float tMax = glm::length(dir);
	return !m_scene->IntersectP(Ray(p0.position, glm::normalize(dir)), tMax - ShadowEpsilon);
}

void Integrator::GenerateTiles() {
	m_tiles.clear();
	glm::ivec2 tileGridSize = glm::ceil(Vec2(m_resolution) / Vec2(m_tileSize));
	for (int32_t yTile = 0; yTile < tileGridSize.y; yTile++) {
		for (int32_t xTile = 0; xTile < tileGridSize.x; xTile++) {
			int32_t xMax = glm::min(m_resolution.x, (xTile + 1) * m_tileSize.x);
			int32_t yMax = glm::min(m_resolution.y, (yTile + 1) * m_tileSize.y);
			glm::ivec2 min = glm::ivec2(xTile * m_tileSize.x, yTile * m_tileSize.y);
			glm::ivec2 max = glm::ivec2(xMax, yMax);
			m_tiles.push_back(Bounds2i(min, max));
		}
	}
}

uint32_t Integrator::GetThreadsCount() {
	return m_threadsCount;
}

uint32_t Integrator::GetSamplesCount() {
	return m_samples;
}

float Integrator::GetRenderTime() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return (newTime - m_renderStartTime).count() / 1000.0f;
}

float Integrator::GetLastSampleTime() {
	return m_lastSampleTime.count() / 1000.0f;
}

CameraSample Integrator::GetCameraSample(uint32_t x, uint32_t y, const SampleFilter* filter, Sampler* sampler) {
	FilterSample fs = filter->Sample(sampler->GetPixel2D());
	return CameraSample(Vec2(x, y) + fs.p + Vec2(0.5f, 0.5f), fs.weight);
}
