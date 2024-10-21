#pragma once
#include "pch.h"
#include "Component.h"
#include "MeshAnimator.h"
#include "OpenGLInterface.h"

class MeshAnimatorComponent : public Component {
public:
	MeshAnimatorComponent(const std::vector<Animation*>& animations, SceneObject* parent, Mat4 globalInverseTransform);
	~MeshAnimatorComponent();

	void UpdateAnimation(Float deltaTime);
	std::vector<Mat4> GetBoneMatrices();

protected:
	Animator* m_animator;
	std::vector<Animation*> m_animations;
	int32_t m_currentAnimation = -1;
};
