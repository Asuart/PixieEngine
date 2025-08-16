#pragma once
#include "pch.h"
#include "Camera.h"
#include "UserInput/UserInput.h"

namespace PixieEngine {

class OrbitalCameraController {
public:
	OrbitalCameraController(Camera& camera, float moveSpeed = 1.0f);

	// Return true if camera moved.
	bool HandleUserInput();

protected:
	Camera& m_camera;
	float m_moveSpeed;
};

}