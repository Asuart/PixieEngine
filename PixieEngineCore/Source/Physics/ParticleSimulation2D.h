#pragma once
#include "pch.h"
#include "Utils/StringUtils.h"

class ParticleSimulation2D {
public:
	ParticleSimulation2D();

	bool Init();
	void Step();
	void CreateParticle(glm::vec2 position, float size, float mass);
	void CreateSpring(uint32_t p0, uint32_t p1, float strength);
	void FetchPositions();
	void FetchCellHashes();

public:
	// Particles data
	int32_t m_numParticles = 0;
	std::vector<glm::vec2> m_positions;
	std::vector<glm::vec2> m_lastPositions;
	std::vector<glm::vec2> m_forces;
	std::vector<float> m_masses;
	std::vector<float> m_sizes;
	// Grid data
	std::vector<int32_t> m_cellHashes;
	std::vector<int32_t> m_cellIndexes;
	std::vector<int32_t> m_cellStarts;
	// Srings data
	int32_t m_numSprings = 0;
	std::vector<int32_t> m_springsStarts;
	std::vector<int32_t> m_springsEnds;
	std::vector<float> m_springsLengths;
	std::vector<float> m_springsStrengths;
	// Compute Shaders Data
	GLuint m_updateHashesShader = 0;
	GLuint m_sortHashesShader = 0;
	GLuint m_resetHashesStartsShader = 0;
	GLuint m_updateHashesStartsShader = 0;
	GLuint m_resetForcesShader = 0;
	GLuint m_checkCollisionsShader = 0;
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
	static constexpr GLuint m_positionsBinding = 0;
	static constexpr GLuint m_lastPositionsBinding = 1;
	static constexpr GLuint m_forcesBinding = 2;
	static constexpr GLuint m_massesBinding = 3;
	static constexpr GLuint m_sizesBinding = 4;
	static constexpr GLuint m_cellHashesBinding = 5;
	static constexpr GLuint m_cellIndexesBinding = 6;
	static constexpr GLuint m_cellStartsBinding = 7;
	static constexpr GLuint m_springsStartsBinding = 8;
	static constexpr GLuint m_springsEndsBinding = 9;
	static constexpr GLuint m_springsLengthsBinding = 10;
	static constexpr GLuint m_springsStrengthsBinding = 11;

	// World Settings
	glm::vec2 m_bounds = glm::vec2(200.0f, 200.0);
	glm::vec2 m_cellSize = glm::vec2(2.0f, 2.0f);
	glm::ivec2 m_gridResolution = glm::ivec2(100, 100);
	float m_largestParticle = 1.0f;
	float m_stepDeltaTime = 0.001f;
	float m_stepDamping = 0.9998f;
	float m_collisionAcceleration = 1000.0f;
	float m_gravity = 9.81f;
	float m_epsilon = 0.001f;
	int32_t m_localWorkGroupSize = 16;

	void UpdateHashes();
	void SortHashes();
	void ResetHashesStarts();
	void UpdateHashesStarts();
	void ResetForces();
	void UpdateSprings();
	void CheckCollisions();
	void UpdatePositions();

	GLuint CompileComputeShader(const char* computeShaderSource);
};
