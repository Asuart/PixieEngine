#pragma once
#include "pch.h"

struct Medium {

};

struct MediumInterface {
	Medium* inside = nullptr;
	Medium* outside = nullptr;
};