#pragma once 

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
