#include "pch.h"
#include "PhysicsEngine.h"
#include "ObjectVsBroadPhaseLayerFilterImplementation.h"
#include "MyBodyActivationListener.h"
#include "MyContactListener.h"
#include "EngineTime.h"

using namespace JPH;
using namespace JPH::literals;

float PhysicsEngine::m_timeAlign;
TempAllocatorImpl* m_tempAllocator = nullptr;
JobSystemThreadPool* m_jobSystem = nullptr;
PhysicsSystem* m_physicsSystem = nullptr;
BPLayerInterfaceImplementation* m_broadPhaseLayerInterface = nullptr;
ObjectVsBroadPhaseLayerFilterImplementation* m_objectVsBroadphaseLayerFilter = nullptr;
ObjectLayerPairFilterImplementation* m_objectVsObjectLayerFilter = nullptr;
MyBodyActivationListener* m_bodyActivationListener = nullptr;
MyContactListener* m_contactListener = nullptr;
std::map<uint32_t, JPH::BodyID> m_bodiesMap;

bool PhysicsEngine::Initialize() {
	m_timeAlign = 0;

	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();

	m_tempAllocator = new TempAllocatorImpl(1024 * 1024 * 1024);
	m_jobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	m_physicsSystem = new PhysicsSystem();
	m_broadPhaseLayerInterface = new BPLayerInterfaceImplementation();
	m_objectVsBroadphaseLayerFilter = new ObjectVsBroadPhaseLayerFilterImplementation();
	m_objectVsObjectLayerFilter = new ObjectLayerPairFilterImplementation();
	m_physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *m_broadPhaseLayerInterface, *m_objectVsBroadphaseLayerFilter, *m_objectVsObjectLayerFilter);
	
	m_bodyActivationListener = new MyBodyActivationListener();
	m_contactListener = new MyContactListener();
	m_physicsSystem->SetBodyActivationListener(m_bodyActivationListener);
	m_physicsSystem->SetContactListener(m_contactListener);

	m_physicsSystem->OptimizeBroadPhase();

	return true;
}

void PhysicsEngine::Free() {
	BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	for (std::pair<const uint32_t, BodyID> entry : m_bodiesMap) {
		bodyInterface.RemoveBody(entry.second);
		bodyInterface.DestroyBody(entry.second);
	}

	delete m_bodyActivationListener;
	m_bodyActivationListener = nullptr;
	delete m_contactListener;
	m_contactListener = nullptr;
	delete m_objectVsObjectLayerFilter;
	m_objectVsObjectLayerFilter = nullptr;
	delete m_objectVsBroadphaseLayerFilter;
	m_objectVsBroadphaseLayerFilter = nullptr;
	delete m_broadPhaseLayerInterface;
	m_broadPhaseLayerInterface = nullptr;
	delete m_physicsSystem;
	m_physicsSystem = nullptr;
	delete m_jobSystem;
	m_jobSystem = nullptr;
	delete m_tempAllocator;
	m_tempAllocator = nullptr;

	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}

uint32_t PhysicsEngine::CreateSphereBody(glm::vec3 position, float radius, bool activate) {
	BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	SphereShapeSettings settings = SphereShapeSettings(radius);
	settings.SetEmbedded();
	ShapeSettings::ShapeResult shapeResult = settings.Create();
	ShapeRefC shapeRef = shapeResult.Get();
	BodyCreationSettings bodySettings(shapeRef, RVec3(position.x, position.y, position.z), Quat::sIdentity(), activate ? EMotionType::Dynamic : EMotionType::Static, activate ? Layers::MOVING : Layers::NON_MOVING);
	Body* body = bodyInterface.CreateBody(bodySettings);
	body->SetShapeInternal(shapeRef, false);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);
	m_bodiesMap[(uint32_t)m_bodiesMap.size()] = body->GetID();
	return (uint32_t)(m_bodiesMap.size() - 1);
}

glm::vec3 PhysicsEngine::GetBodyPosition(uint32_t id) {
	if (m_bodiesMap.contains(id)) {
		BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
		JPH::RVec3 pos = bodyInterface.GetPosition(m_bodiesMap[id]);
		return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
	}
	return glm::vec3(0.0f);
}

void PhysicsEngine::DestroyBody(uint32_t id) {
	if (m_bodiesMap.contains(id)) {
		BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
		bodyInterface.RemoveBody(m_bodiesMap[id]);
		bodyInterface.DestroyBody(m_bodiesMap[id]);
	}
}

void PhysicsEngine::Update() {
	m_physicsSystem->OptimizeBroadPhase();
	m_timeAlign += Time::deltaTime;
	uint step = 0;
	while (m_timeAlign >= cTimeStep) {
		m_timeAlign -= cTimeStep;
		step++;
		const int cCollisionSteps = 1;
		m_physicsSystem->Update(cTimeStep, cCollisionSteps, m_tempAllocator, m_jobSystem);
	}
}
