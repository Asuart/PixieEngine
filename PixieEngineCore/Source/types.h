#pragma once 

// Define precision of some Floating point values and Floating point vectors as 'Float' or 'double'
#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
typedef double Float;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;
typedef glm::dmat3 Mat3;
typedef glm::dmat4 Mat4;
typedef glm::dquat Quaternion;
#define GL_FLOAT_TYPE GL_DOUBLE
#else
typedef float Float;
typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;
typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;
typedef glm::fquat Quaternion;
#define GL_FLOAT_TYPE GL_FLOAT
#endif
