#pragma once
#include "pch.h"
#include "JoltInterface.h"

class MyContactListener : public JPH::ContactListener {
public:
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override {
		std::cout << "Contact validate callback" << std::endl;
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {
		std::cout << "A contact was added" << std::endl;
	}

	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override {
		std::cout << "A contact was persisted" << std::endl;
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override {
		std::cout << "A contact was removed" << std::endl;
	}
};
