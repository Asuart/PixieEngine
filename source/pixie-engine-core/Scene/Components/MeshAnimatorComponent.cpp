#include "pch.h"
#include "MeshAnimatorComponent.h"

namespace PixieEngine {

MeshAnimatorComponent::MeshAnimatorComponent(SceneObject* parent, const std::vector<Animation*>& animations, glm::mat4 globalInverseTransform) :
	Component(ComponentType::MeshAnimator, parent), m_animations(animations), m_animator(nullptr) {
	if (animations.size() > 0) {
		m_animator = new Animator(animations[0], globalInverseTransform);
	}
}

MeshAnimatorComponent::~MeshAnimatorComponent() {
	for (int32_t i = 0; i < m_animations.size(); i++) {
		delete m_animations[i];
	}
}

void MeshAnimatorComponent::OnUpdate() {
	UpdateAnimation(Time::deltaTime);
}

const std::array<glm::mat4, cMaxBonesPerModel>* MeshAnimatorComponent::GetBoneMatrices() const {
	if (!m_animator) {
		return nullptr;
	}
	return m_animator->GetFinalBoneMatrices();
}

void MeshAnimatorComponent::UpdateAnimation(float dt) {
	m_animator->UpdateAnimation(dt);
}

}