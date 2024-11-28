#pragma once
#include "Component.h"
#include "Camera.h"

class CameraComponent : public Component {
public:
	CameraComponent(SceneObject* parent, glm::ivec2 resolution, Float fovy, Float near, Float far);

	Camera& GetCamera();

protected:
	Camera m_camera;
};
