#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <random>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <map>
#include <span>
#include <atomic>
#include <thread>
#include <optional>
#include <mutex>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Define precision of some floating point values and floating point vectors as 'float' or 'double'
#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
typedef double Float;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;
#else
typedef float Float;
typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;
#endif