#include "pch.h"
#include "ViewportCameraController.h"

ViewportCameraController::ViewportCameraController(Camera& camera) :
	m_camera(camera) {}

bool ViewportCameraController::Update() {
	const float scale = 10.0f * (UserInput::GetKey(GLFW_KEY_LEFT_SHIFT) ? 0.1f : 1.0f) * Time::deltaTime;
	const float rotationScale = (UserInput::GetKey(GLFW_KEY_LEFT_SHIFT) ? 0.1f : 1.0f) * Time::deltaTime;
	bool moved = false;
	Transform& transform = m_camera.GetTransform();
	if (UserInput::GetKey(GLFW_KEY_W)) {
		transform.Translate(transform.GetForward() * scale);
		moved = true;
	}
	if (UserInput::GetKey(GLFW_KEY_S)) {
		transform.Translate(transform.GetForward() * -scale);
		moved = true;
	}
	if (UserInput::GetKey(GLFW_KEY_D)) {
		transform.Translate(transform.GetRight() * scale);
		moved = true;
	}
	if (UserInput::GetKey(GLFW_KEY_A)) {
		transform.Translate(transform.GetRight() * -scale);
		moved = true;
	}
	if (UserInput::GetKey(GLFW_KEY_SPACE)) {
		transform.Translate(transform.GetUp() * scale);
		moved = true;
	}
	if (UserInput::GetKey(GLFW_KEY_LEFT_CONTROL)) {
		transform.Translate(transform.GetUp() * -scale);
		moved = true;
	}
	if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_2)) {
		if (UserInput::mouseDeltaX) {
			transform.RotateAroundAxis(glm::vec3(0, 1, 0), -(float)UserInput::mouseDeltaX * rotationScale);
			transform.LookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), glm::vec3(0, 1, 0));
			moved = true;
		}
		if (UserInput::mouseDeltaY) {
			transform.RotateAroundAxis(glm::vec3(1, 0, 0), -(float)UserInput::mouseDeltaY * rotationScale);
			transform.LookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), glm::vec3(0, 1, 0));
			moved = true;
		}
	}
	return moved;
}
