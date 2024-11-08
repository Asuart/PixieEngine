#include "pch.h"
#include "LightSampler.h"

SampledLight::SampledLight(Light* light, Float p)
	: light(light), p(p) {}
