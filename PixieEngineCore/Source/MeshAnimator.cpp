#include "MeshAnimator.h"

Bone::Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel)
    : name(name), id(ID), localTransform(1.0f) {
    numPositions = channel->mNumPositionKeys;
    for (int32_t positionIndex = 0; positionIndex < numPositions; positionIndex++) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
        data.timeStamp = timeStamp;
        positions.push_back(data);
    }

    numRotations = channel->mNumRotationKeys;
    for (int32_t rotationIndex = 0; rotationIndex < numRotations; rotationIndex++) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timeStamp = timeStamp;
        rotations.push_back(data);
    }

    numScalings = channel->mNumScalingKeys;
    for (int32_t keyIndex = 0; keyIndex < numScalings; keyIndex++) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec(scale);
        data.timeStamp = timeStamp;
        scales.push_back(data);
    }
}

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

glm::mat4 Bone::GetLocalTransform() const {
    return localTransform;
}

std::string Bone::GetBoneName() const {
    return name;
}

int32_t Bone::GetBoneID() const {
    return id;
}

int32_t Bone::GetPositionIndex(float animationTime) const {
    for (int index = 0; index < numPositions - 1; index++) {
        if (animationTime < positions[index + 1].timeStamp)
            return index;
    }
}

int32_t Bone::GetRotationIndex(float animationTime) const {
    for (int index = 0; index < numRotations - 1; index++) {
        if (animationTime < rotations[index + 1].timeStamp)
            return index;
    }
}

int32_t Bone::GetScaleIndex(float animationTime) const {
    for (int index = 0; index < numScalings - 1; index++) {
        if (animationTime < scales[index + 1].timeStamp)
            return index;
    }
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (numPositions == 1)
        return glm::translate(glm::mat4(1.0f), positions[0].position);

    int32_t p0Index = GetPositionIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (numRotations == 1) {
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
    if (numScalings == 1)
        return glm::scale(glm::mat4(1.0f), scales[0].scale);

    int32_t p0Index = GetScaleIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

Animation::Animation(const std::string& animationPath, Skeleton* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Animation::~Animation() {}

Bone* Animation::FindBone(const std::string& name) {
    auto iter = std::find_if(bones.begin(), bones.end(),
        [&](const Bone& Bone) {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == bones.end())
        return nullptr;
    else
        return &(*iter);
}


float Animation::GetTicksPerSecond() const {
    return ticksPerSecond;
}

float Animation::GetDuration() const {
    return duration;
}

const AssimpNodeData& Animation::GetRootNode() const {
    return rootNode;
}

const std::map<std::string, BoneInfo>& Animation::GetBoneIDMap() const {
    return boneInfoMap;
}

void Animation::ReadMissingBones(const aiAnimation* animation, Skeleton& model) {
    int32_t size = animation->mNumChannels;

    std::map<std::string, BoneInfo>& newBoneInfoMap = model.GetBoneInfoMap();
    int32_t& boneCount = model.GetBoneCount();

    for (int32_t i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (newBoneInfoMap.find(boneName) == newBoneInfoMap.end()) {
            newBoneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.push_back(Bone(channel->mNodeName.data,
            newBoneInfoMap[channel->mNodeName.data].id, channel));
    }

    boneInfoMap = newBoneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int32_t i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

Animator::Animator(Animation* Animation) {
    currentTime = 0.0;
    currentAnimation = Animation;

    finalBoneMatrices.reserve(100);

    for (int32_t i = 0; i < 100; i++)
        finalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::UpdateAnimation(float dt) {
    deltaTime = dt;
    if (currentAnimation) {
        currentTime += currentAnimation->GetTicksPerSecond() * dt;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());
        CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* pAnimation) {
    currentAnimation = pAnimation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(currentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int32_t index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int32_t i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
    return finalBoneMatrices;
}