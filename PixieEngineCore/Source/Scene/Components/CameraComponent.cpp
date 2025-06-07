#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(SceneObject* parent, float aspect, float fovy, float near, float far) 
	: Component(ComponentType::Camera, parent), m_camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), fovy, aspect, near, far) {}

Camera& CameraComponent::GetCamera() {
	return m_camera;
}
