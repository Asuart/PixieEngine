#pragma once
#include "pch.h"
#include "JoltInterface.h"

class MyBodyActivationListener : public JPH::BodyActivationListener {
public:
	virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {
		std::cout << "A body got activated" << std::endl;
	}

	virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override {
		std::cout << "A body went to sleep" << std::endl;
	}
};