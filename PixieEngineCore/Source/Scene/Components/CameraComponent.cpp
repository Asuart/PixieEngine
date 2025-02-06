#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(SceneObject* parent, glm::ivec2 resolution, Float fovy, Float near, Float far) :
	Component(ComponentType::Camera, parent),
	m_camera(Vec3(0, 0, 0), Vec3(0, 0, 1), Vec3(0, 1, 0), fovy, { 1280, 720 }, 10.0f, near, far) {}

Camera& CameraComponent::GetCamera() {
	return m_camera;
}

