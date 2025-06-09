#pragma once
#include "pch.h"
#include "Utils/StringUtils.h"

class ParticleSimulation2D {
public:
	ParticleSimulation2D();

	bool Init();
	void Free();
	void UploadData();
	void ResetParticlesAndSprings();
	void Step();
	void SetBoundsSize(glm::vec2 bounds);
	void CreateParticle(glm::vec2 position, float size, float mass);
	void CreateSpring(uint32_t p0, uint32_t p1, float strength);

	std::vector<glm::vec2> FetchPositions() const;
	std::vector<glm::vec2> FetchLastPositions() const;
	std::vector<glm::vec2> FetchForces() const;
	std::vector<float> FetchMasses() const;
	std::vector<float> FetchSizes() const;
	std::vector<int32_t> FetchCellHashes() const;
	std::vector<int32_t> FetchCellIndexes() const;
	std::vector<int32_t> FetchCellStarts() const;
	std::vector<int32_t> FetchSpringsStarts() const;
	std::vector<int32_t> FetchSpringsEnds() const;
	std::vector<float> FetchSpringsLengths() const;
	std::vector<float> FetchSpringsStrengths() const;

public:
	bool m_initialized = false;
	int32_t m_localWorkGroupSize = 16;
	// Particles data
	int32_t m_numParticlesOnGPU = 0;
	std::vector<glm::vec2> m_positions;
	std::vector<glm::vec2> m_lastPositions;
	std::vector<glm::vec2> m_forces;
	std::vector<float> m_masses;
	std::vector<float> m_sizes;
	// Grid data
	int32_t m_numCellStartsOnGPU = 0;
	std::vector<int32_t> m_cellHashes;
	std::vector<int32_t> m_cellIndexes;
	std::vector<int32_t> m_cellStarts;
	// Srings data
	int32_t m_numSpringsOnGPU = 0;
	std::vector<int32_t> m_springsStarts;
	std::vector<int32_t> m_springsEnds;
	std::vector<float> m_springsLengths;
	std::vector<float> m_springsStrengths;
	std::vector<int32_t> m_springsGroups;
	// Compute Shaders Data
	GLuint m_updateHashesShader = 0;
	GLuint m_sortHashesShader = 0;
	GLuint m_resetHashesStartsShader = 0;
	GLuint m_updateHashesStartsShader = 0;
	GLuint m_resetForcesShader = 0;
	GLuint m_checkCollisionsShader = 0;
	GLuint m_updateSpringsShader = 0;
	GLuint m_updatePositionsShader = 0;
	GLuint m_positionsBuffer = 0;
	GLuint m_lastPositionsBuffer = 0;
	GLuint m_forcesBuffer = 0;
	GLuint m_massesBuffer = 0;
	GLuint m_sizesBuffer = 0;
	GLuint m_cellHashesBuffer = 0;
	GLuint m_cellIndexesBuffer = 0;
	GLuint m_cellStartsBuffer = 0;
	GLuint m_springsStartsBuffer = 0;
	GLuint m_springsEndsBuffer = 0;
	GLuint m_springsLengthsBuffer = 0;
	GLuint m_springsStrengthsBuffer = 0;
	// Bindings
	enum BufferBinding : GLuint {
		Position = 0,
		LastPosition,
		Forces,
		Masses,
		Sizes,
		CellHashes,
		CellIndexes,
		CellStarts,
		SpringsStarts,
		SpringsEnds,
		SpringsLengths,
		SpringsStrengths,
	};
	// World Settings
	glm::vec2 m_bounds = glm::vec2(200.0f, 200.0);
	glm::vec2 m_cellSize = glm::vec2(200.0f, 200.0f);
	glm::ivec2 m_gridResolution = glm::ivec2(1, 1);
	float m_largestParticle = 1.0f;
	float m_stepDeltaTime = 1.0f / 960.0f;
	float m_stepDamping = 0.995f;
	float m_collisionAcceleration = 100.0f;
	float m_gravity = 981.0f;
	float m_epsilon = 0.001f;

	void UpdateHashes();
	void SortHashes();
	void ResetHashesStarts();
	void UpdateHashesStarts();
	void ResetForces();
	void UpdateSprings();
	void CheckCollisions();
	void UpdatePositions();

	GLuint CompileComputeShader(const char* computeShaderSource);
	void ColorSprings();
	void CompileShaders();
};
