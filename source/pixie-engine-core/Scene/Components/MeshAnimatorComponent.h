#pragma once
#include "pch.h"
#include "Component.h"
#include "Animation/MeshAnimator.h"
#include "EngineTime.h"

class MeshAnimatorComponent : public Component {
public:
	MeshAnimatorComponent(SceneObject* parent, const std::vector<Animation*>& animations, glm::mat4 globalInverseTransform);
	~MeshAnimatorComponent();

	void OnUpdate() override;
	const std::array<glm::mat4, cMaxBonesPerModel>* GetBoneMatrices() const;

protected:
	Animator* m_animator;
	std::vector<Animation*> m_animations;
	int32_t m_currentAnimation = -1;

	void UpdateAnimation(float deltaTime);
};
