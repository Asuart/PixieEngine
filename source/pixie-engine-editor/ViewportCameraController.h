#pragma once
#include "pch.h"
#include "UserInput.h"

class ViewportCameraController {
public:
	ViewportCameraController(Camera& camera, float moveSpeed = 1.0f);

	// Return true if camera moved.
	bool Update();

protected:
	Camera& m_camera;
	float m_moveSpeed;
};
