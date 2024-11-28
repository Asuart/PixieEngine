#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
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
#include <set>
#include <span>
#include <atomic>
#include <thread>
#include <optional>
#include <mutex>
#include <numeric>
#include <memory_resource>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
typedef double Float;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;
typedef glm::dmat3 Mat3;
typedef glm::dmat4 Mat4;
typedef glm::dquat Quaternion;
typedef GLdouble GLFloatType;
#define GL_FLOAT_TYPE GL_DOUBLE
#else
typedef float Float;
typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;
typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;
typedef glm::fquat Quaternion;
typedef GLfloat GLFloatType;
#define GL_FLOAT_TYPE GL_FLOAT
#endif

#include "OpenGLInterface.h"
