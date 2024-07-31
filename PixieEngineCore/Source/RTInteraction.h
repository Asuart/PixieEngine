#pragma once
#include "headers.h"

class Material;
class RTMaterial;
class Shape;
class Mesh;

struct InteractionBase {
	glm::vec3 p = glm::vec3(0);
	glm::vec3 n = glm::vec3(0);
	glm::vec2 uv = glm::vec2(0);
	float distance = 0;
	bool backFace = false;
};

struct RayInteraction : InteractionBase {
	Material* material = nullptr;
	Mesh* mesh = nullptr;
	int32_t triangleIndex = -1;
};

struct RTInteraction : InteractionBase {
	glm::vec3 wo = glm::vec3(0);
	glm::vec3 dpdus = glm::vec3(0);
	RTMaterial* material = 0;
	Shape* shape = nullptr;
	float area = 0;
};
