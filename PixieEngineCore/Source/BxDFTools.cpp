
#include "BxDFTools.h"

bool IsReflective(BxDFFlags f) {
	return f & BxDFFlags::Reflection;
}

bool IsTransmissive(BxDFFlags f) {
	return f & BxDFFlags::Transmission;
}

bool IsDiffuse(BxDFFlags f) {
	return f & BxDFFlags::Diffuse;
}

bool IsGlossy(BxDFFlags f) {
	return f & BxDFFlags::Glossy;
}

bool IsSpecular(BxDFFlags f) {
	return f & BxDFFlags::Specular;
}

bool IsNonSpecular(BxDFFlags f) {
	return f & (BxDFFlags::Diffuse | BxDFFlags::Glossy);
}