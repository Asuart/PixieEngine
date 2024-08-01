#pragma once 

// Define precision of some Floating point values and Floating point vectors as 'Float' or 'double'
// Double mode is not ready
#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
typedef double Float;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;
typedef glm::dmat4 Mat4;
#else
typedef float Float;
typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;
typedef glm::fmat4 Mat4;
#endif
