#pragma once
#include "pch.h"
#include "UserInput.h"

class ViewportCameraController {
public:
	ViewportCameraController(Camera& camera);

	// Return true if camera moved.
	bool Update();

protected:
	Camera& m_camera;
};
