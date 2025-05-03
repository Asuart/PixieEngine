#include "pch.h"
#include "PhysicsEngine.h"
#include "ObjectVsBroadPhaseLayerFilterImplementation.h"
#include "MyBodyActivationListener.h"
#include "MyContactListener.h"
#include "EngineTime.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;

Float PhysicsEngine::m_timeAlign = 0;

TempAllocatorImpl* m_tempAllocator = nullptr;
JobSystemThreadPool* m_jobSystem = nullptr;
PhysicsSystem* m_physicsSystem = nullptr;
BPLayerInterfaceImplementation* m_broadPhaseLayerInterface = nullptr;
ObjectVsBroadPhaseLayerFilterImplementation* m_objectVsBroadphaseLayerFilter = nullptr;
ObjectLayerPairFilterImplementation* m_objectVsObjectLayerFilter = nullptr;
MyBodyActivationListener* m_bodyActivationListener = nullptr;
MyContactListener* m_contactListener = nullptr;
std::map<uint32_t, JPH::BodyID> bodiesMap;

void PhysicsEngine::Initialize() {
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
	
	//m_bodyActivationListener = new MyBodyActivationListener();
	//m_contactListener = new MyContactListener();
	//m_physicsSystem->SetBodyActivationListener(m_bodyActivationListener);
	//m_physicsSystem->SetContactListener(m_contactListener);

	m_physicsSystem->OptimizeBroadPhase();
}

uint32_t PhysicsEngine::CreateSphereBody(::Vec3 position, Float radius, bool activate) {
	BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	SphereShapeSettings settings = SphereShapeSettings(radius);
	settings.SetEmbedded();
	ShapeSettings::ShapeResult shapeResult = settings.Create();
	ShapeRefC shapeRef = shapeResult.Get();
	BodyCreationSettings bodySettings(shapeRef, RVec3(position.x, position.y, position.z), Quat::sIdentity(), activate ? EMotionType::Dynamic : EMotionType::Static, activate ? Layers::MOVING : Layers::NON_MOVING);
	Body* body = bodyInterface.CreateBody(bodySettings);
	body->SetShapeInternal(shapeRef, false);
	bodyInterface.AddBody(body->GetID(), EActivation::Activate);
	bodiesMap[bodiesMap.size()] = body->GetID();
	return bodiesMap.size() - 1;
}

::Vec3 PhysicsEngine::GetBodyPosition(uint32_t id) {
	BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	JPH::RVec3 pos = bodyInterface.GetPosition(bodiesMap[id]);
	return ::Vec3(pos.GetX(), pos.GetY(), pos.GetZ());
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

void PhysicsEngine::Free() {
	// Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
	//body_interface.RemoveBody(sphere_id);

	// Destroy the sphere. After this the sphere ID is no longer valid.
	//body_interface.DestroyBody(sphere_id);

	// Remove and destroy the floor
	//body_interface.RemoveBody(floor->GetID());
	//body_interface.DestroyBody(floor->GetID());

	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}
