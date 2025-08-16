#pragma once
#include "Component.h"
#include "Scene/Camera.h"

namespace PixieEngine {

class CameraComponent : public Component {
public:
	CameraComponent(SceneObject* parent, float aspect, float fovy, float near, float far);

	Camera& GetCamera();

protected:
	Camera m_camera;
};

}