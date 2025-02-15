#include "pch.h"
#include "PhysicsEngine.h"
#include "ObjectVsBroadPhaseLayerFilterImplementation.h"
#include "MyBodyActivationListener.h"
#include "MyContactListener.h"

using namespace JPH;
using namespace JPH::literals;
using namespace std;

Float PhysicsEngine::m_timeAlign = 0;

TempAllocatorImpl m_tempAllocator(10 * 1024 * 1024);
JobSystemThreadPool m_jobSystem;
PhysicsSystem m_physicsSystem;
BPLayerInterfaceImplementation m_broadPhaseLayerInterface;
ObjectVsBroadPhaseLayerFilterImplementation m_objectVsBroadphaseLayerFilter;
ObjectLayerPairFilterImplementation m_objectVsObjectLayerFilter;
MyBodyActivationListener m_bodyActivationListener;
MyContactListener m_contactListener;

void PhysicsEngine::Initialize() {
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();

	m_physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, m_broadPhaseLayerInterface, m_objectVsBroadphaseLayerFilter, m_objectVsObjectLayerFilter);
	m_physicsSystem.SetBodyActivationListener(&m_bodyActivationListener);
	m_physicsSystem.SetContactListener(&m_contactListener);

	BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

	//BoxShapeSettings floorShapeSettings(RVec3(100.0f, 1.0f, 100.0f));
	//floorShapeSettings.SetEmbedded();

	//ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings.Create();
	//ShapeRefC floorShape = floorShapeResult.Get();

	//BodyCreationSettings floor_settings(floorShape, RVec3(0.0_r, -1.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//Body* floor = bodyInterface.CreateBody(floor_settings);

	//bodyInterface.AddBody(floor->GetID(), EActivation::DontActivate);

	//BodyCreationSettings sphereSettings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	//BodyID sphere_id = bodyInterface.CreateAndAddBody(sphereSettings, EActivation::Activate);

	//bodyInterface.SetLinearVelocity(sphere_id, RVec3(0.0f, -5.0f, 0.0f));

	m_physicsSystem.OptimizeBroadPhase();
}

void PhysicsEngine::Update(Float deltaTime) {
	m_timeAlign += deltaTime;
	uint step = 0;
	while (m_timeAlign >= cTimeStep) {
		m_timeAlign -= cTimeStep;
		step++;
		const int cCollisionSteps = 1;
		m_physicsSystem.Update(cTimeStep, cCollisionSteps, &m_tempAllocator, &m_jobSystem);
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
