#pragma once
#include "PixieEngineCoreHeaders.h"

struct Medium {

};

struct MediumInterface {
	Medium* inside = nullptr;
	Medium* outside = nullptr;
};