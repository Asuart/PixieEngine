#pragma once
#include "pch.h"
#include "Resources/ShaderManager.h"
#include "Resources/SSBOHandle.h"
#include "Utils/StringUtils.h"
#include "Utils/DynamicTypelessBuffer.h"

class ParticleSimulation3D {
public:
	enum class Buffer : GLuint {
		PositionsX = 0,
		PositionsY,
		PositionsZ,
		LastPositionsX,
		LastPositionsY,
		LastPositionsZ,
		ForcesX,
		ForcesY,
		ForcesZ,
		Masses,
		Sizes,
		CellHashes,
		CellIndexes,
		CellStarts,
		SpringsStarts,
		SpringsEnds,
		SpringsLengths,
		SpringsStrengths,
		Count,
	};

	enum class ShaderProgram : GLuint {
		UpdateHashes = 0,
		SortHashes,
		ResetHashesStarts,
		UpdateHashesStarts,
		ResetForces,
		CheckCollisions,
		UpdateSprings,
		UpdatePositions,
		Count,
	};

	ParticleSimulation3D();
	~ParticleSimulation3D();

	bool Init();
	void Free();
	void UploadData();
	void ResetParticlesAndSprings();
	void Step();
	void SetBoundsSize(glm::dvec3 bounds);
	void CreateParticle(glm::dvec3 position, double size, double mass);
	void CreateSpring(int32_t p0, int32_t p1, double strength);

	std::vector<glm::vec3> FetchPositions() const;
	std::vector<glm::vec3> FetchLastPositions() const;
	std::vector<glm::vec3> FetchForces() const;
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
	// Shaders Configs 
	int32_t m_localWorkGroupSize = 16;
	std::array<ComputeShader, (int32_t)ShaderProgram::Count> m_programs;
	// Main Data
	int32_t m_numParticlesOnGPU = 0;
	int32_t m_numCellStartsOnGPU = 0;
	int32_t m_numSpringsOnGPU = 0;
	int32_t m_numParticlesOnCPU = 0;
	int32_t m_numSpringsOnCPU = 0;
	std::array<SSBOHandle, (int32_t)Buffer::Count> m_gpuBuffers;
	std::array<DynamicTypelessBuffer, (int32_t)Buffer::Count> m_cpuBuffers;
	std::vector<int32_t> m_springsGroups;
	// World Settings
	glm::dvec3 m_bounds = glm::dvec3(200.0, 200.0, 200.0);
	glm::dvec3 m_cellSize = glm::dvec3(200.0, 200.0, 200.0);
	glm::ivec3 m_gridResolution = glm::ivec3(1, 1, 1);
	double m_largestParticle = 1.0f;
	double m_stepDeltaTime = 1.0f / 1000.0f;
	double m_stepDamping = 0.9998f;
	double m_collisionForce = 100.0f;
	double m_gravity = 981.0f;

	void UploadBuffer(Buffer index);

	void UpdateHashes();
	void SortHashes();
	void ResetHashesStarts();
	void UpdateHashesStarts();
	void ResetForces();
	void UpdateSprings();
	void CheckCollisions();
	void UpdatePositions();

	ComputeShader CompileComputeShader(const std::string& computeShaderSource);
	std::string PreprocessShader(const std::string& computeShaderSource);
	void ColorSprings();
	void CompileShaders();
	int32_t GetDispatchSizeX(int32_t numInstances);
};
