#include "pch.h"
#include "MeshAnimator.h"

Bone::Bone(const std::string& name, int32_t ID)
    : name(name), id(ID), localTransform(1.0f) { }

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

int32_t Bone::GetPositionIndex(float animationTime) const {
    for (size_t index = 0; index < positions.size() - 1; index++) {
        if (animationTime < positions[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

int32_t Bone::GetRotationIndex(float animationTime) const {
    for (size_t index = 0; index < rotations.size() - 1; index++) {
        if (animationTime < rotations[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

int32_t Bone::GetScaleIndex(float animationTime) const {
    for (size_t index = 0; index < scales.size() - 1; index++) {
        if (animationTime < scales[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (positions.size() == 1) {
        return glm::translate(glm::mat4(1.0f), positions[0].position);
    }

    int32_t p0Index = GetPositionIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (rotations.size() == 1) {
        auto rotation = glm::normalize(rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int32_t p0Index = GetRotationIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if (scales.size() == 1) {
        return glm::scale(glm::mat4(1.0f), scales[0].scale);
    }

    int32_t p0Index = GetScaleIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

Animation::Animation(float duration, int32_t ticksPerSecond, const std::vector<Bone>& bones, const std::map<std::string, BoneInfo>& boneInfoMap, SceneObject* rootObject)
    : duration(duration), ticksPerSecond(ticksPerSecond), bones(bones), boneInfoMap(boneInfoMap), rootNode(rootObject) {}


Animation::~Animation() {}

Bone* Animation::FindBone(const std::string& name) {
    for (size_t i = 0; i < bones.size(); i++) {
        if (bones[i].name == name) return &bones[i];
    }
    return nullptr;
}

int32_t Animation::GetTicksPerSecond() const {
    return ticksPerSecond;
}

float Animation::GetDuration() const {
    return duration;
}

SceneObject* Animation::GetRootNode() {
    return rootNode;
}

std::map<std::string, BoneInfo>& Animation::GetBoneIDMap() {
    return boneInfoMap;
}

Animator::Animator(Animation* animation, glm::mat4 globalInverseTransform) :
    m_globalInverseTransform(globalInverseTransform), m_currentAnimation(animation) {}

void Animator::UpdateAnimation(float dt) {
    m_deltaTime = dt;
    if (m_currentAnimation) {
        m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
        m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
        CalculateBoneTransform(m_currentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* animation) {
    m_currentAnimation = animation;
    m_currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(SceneObject* node, glm::mat4 parentTransform) {
    std::string nodeName = node->GetName();
    glm::mat4 nodeTransform = node->GetTransform().GetMatrix();

    Bone* bone = m_currentAnimation->FindBone(nodeName);
    if (bone) {
        bone->Update(m_currentTime);
        nodeTransform = bone->localTransform;
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int32_t index = boneInfoMap[nodeName].id;
        if (index < cMaxBonesPerModel) {
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_finalBoneMatrices[index] = m_globalInverseTransform * globalTransformation * offset;
        }
    }

    for (size_t i = 0; i < node->GetChildren().size(); i++) {
        CalculateBoneTransform(node->GetChild((int32_t)i), globalTransformation);
    }
}

const std::array<glm::mat4, cMaxBonesPerModel>* Animator::GetFinalBoneMatrices() const {
    return &m_finalBoneMatrices;
}
