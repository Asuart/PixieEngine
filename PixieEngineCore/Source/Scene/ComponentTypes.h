#pragma once
#include "pch.h"

enum class ComponentType {
	Undefined = 0,
	Material,
	Mesh,
	Softbody,
	Camera,
	PointLight,
	DirectionalLight,
	DiffuseAreaLight,
	MeshAnimator,
	Sphere,
};

constexpr inline std::string to_string(ComponentType type) {
	switch (type) {
	case ComponentType::Undefined: return "Undefined Component";
	case ComponentType::Material: return "Material Component";
	case ComponentType::Mesh: return "Mesh Component";
	case ComponentType::Softbody: return "Softbody Component";
	case ComponentType::Camera: return "Camera Component";
	case ComponentType::PointLight: return "Point Light Component";
	case ComponentType::DirectionalLight: return "Directional Light Component";
	case ComponentType::DiffuseAreaLight: return "Diffuse Area Light Component";
	case ComponentType::MeshAnimator: return "Mesh Animator Component";
	case ComponentType::Sphere: return "Sphere Component";
	default: return "Undefined Component";
	}
};
