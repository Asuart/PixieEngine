#include "pch.h"
#include "MeshAnimatorComponent.h"

MeshAnimatorComponent::MeshAnimatorComponent(const std::vector<Animation*>& animations, SceneObject* parent, Mat4 globalInverseTransform)
	: Component(ComponentType::MeshAnimator, parent), m_animations(animations), m_animator(nullptr) {
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
	UpdateAnimation(Timer::deltaTime);
}

void MeshAnimatorComponent::UpdateAnimation(Float dt) {
	if (m_animator) {
		m_animator->UpdateAnimation(dt);
	}
}

std::vector<Mat4> MeshAnimatorComponent::GetBoneMatrices() {
	if (m_animator) {
		return m_animator->GetFinalBoneMatrices();
	}
	return {};
}

void MeshAnimatorComponent::GetBoneMatrices(Float time, std::array<Mat4, MaxBonesPerModel>& transforms) const {
	if (!m_animator) {
		return;
	}
	m_animator->GetBoneMatrices(time, transforms);
}
