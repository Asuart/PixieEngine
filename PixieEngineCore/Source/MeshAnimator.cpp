
#include "MeshAnimator.h"

Bone::Bone(const std::string& name, int32_t ID)
    : name(name), id(ID), localTransform(1.0f) { }

void Bone::Update(Float animationTime) {
    Mat4 translation = InterpolatePosition(animationTime);
    Mat4 rotation = InterpolateRotation(animationTime);
    Mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

int32_t Bone::GetPositionIndex(Float animationTime) const {
    for (size_t index = 0; index < positions.size() - 1; index++) {
        if (animationTime < positions[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

int32_t Bone::GetRotationIndex(Float animationTime) const {
    for (size_t index = 0; index < rotations.size() - 1; index++) {
        if (animationTime < rotations[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

int32_t Bone::GetScaleIndex(Float animationTime) const {
    for (size_t index = 0; index < scales.size() - 1; index++) {
        if (animationTime < scales[index + 1].timeStamp) {
            return (int32_t)index;
        }
    }
    return -1;
}

Float Bone::GetScaleFactor(Float lastTimeStamp, Float nextTimeStamp, Float animationTime) {
    Float midWayLength = animationTime - lastTimeStamp;
    Float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

Mat4 Bone::InterpolatePosition(Float animationTime) {
    if (positions.size() == 1) {
        return glm::translate(Mat4(1.0f), positions[0].position);
    }

    int32_t p0Index = GetPositionIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    Float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    Vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
    return glm::translate(Mat4(1.0f), finalPosition);
}

Mat4 Bone::InterpolateRotation(Float animationTime) {
    if (rotations.size() == 1) {
        auto rotation = glm::normalize(rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int32_t p0Index = GetRotationIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    Float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, (float)scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

Mat4 Bone::InterpolateScaling(Float animationTime) {
    if (scales.size() == 1) {
        return glm::scale(Mat4(1.0f), scales[0].scale);
    }

    int32_t p0Index = GetScaleIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    Float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    Vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(Mat4(1.0f), finalScale);
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

Float Animation::GetDuration() const {
    return duration;
}

const SceneObject* Animation::GetRootNode() const {
    return rootNode;
}

const std::map<std::string, BoneInfo>& Animation::GetBoneIDMap() const {
    return boneInfoMap;
}

Animator::Animator(Animation* Animation) {
    currentTime = 0.0;
    currentAnimation = Animation;

    finalBoneMatrices.reserve(100);

    for (uint32_t i = 0; i < 100; i++) {
        finalBoneMatrices.push_back(Mat4(1.0f));
    }
}

void Animator::UpdateAnimation(Float dt) {
    deltaTime = dt;
    if (currentAnimation) {
        currentTime += currentAnimation->GetTicksPerSecond() * dt;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());
        CalculateBoneTransform(currentAnimation->GetRootNode(), Mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* pAnimation) {
    currentAnimation = pAnimation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const SceneObject* node, Mat4 parentTransform) {
    std::string nodeName = node->name;
    Mat4 nodeTransform = node->transform.GetMatrix();

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(currentTime);
        nodeTransform = Bone->localTransform;
    }

    Mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int32_t index = boneInfoMap[nodeName].id;
        Mat4 offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (size_t i = 0; i < node->children.size(); i++) {
        CalculateBoneTransform(node->children[i], globalTransformation);
    }
}

std::vector<Mat4> Animator::GetFinalBoneMatrices() {
    return finalBoneMatrices;
}