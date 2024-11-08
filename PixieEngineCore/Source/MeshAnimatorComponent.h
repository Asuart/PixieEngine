#pragma once
#include "pch.h"
#include "Component.h"
#include "MeshAnimator.h"
#include "OpenGLInterface.h"
#include "Timer.h"

class MeshAnimatorComponent : public Component {
public:
	MeshAnimatorComponent(const std::vector<Animation*>& animations, SceneObject* parent, Mat4 globalInverseTransform);
	~MeshAnimatorComponent();

	void OnUpdate() override;
	void UpdateAnimation(Float deltaTime);
	std::vector<Mat4> GetBoneMatrices();
	void GetBoneMatrices(Float time, std::array<Mat4, MaxBonesPerModel>& transforms) const;

protected:
	Animator* m_animator;
	std::vector<Animation*> m_animations;
	int32_t m_currentAnimation = -1;
};
