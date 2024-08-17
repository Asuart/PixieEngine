
#include "BSDFSample.h"

BSDFSample::BSDFSample(Spectrum f, Vec3 wi, Float pdf, BxDFFlags flags, Float eta, bool pdfIsProportional)
	: f(f), wi(wi), pdf(pdf), flags(flags), eta(eta), pdfIsProportional(pdfIsProportional) {}

bool BSDFSample::IsReflection() const {
	return ::IsReflective(flags);
}

bool BSDFSample::IsTransmission() const {
	return ::IsTransmissive(flags);
}

bool BSDFSample::IsDiffuse() const {
	return ::IsDiffuse(flags);
}

bool BSDFSample::IsGlossy() const {
	return ::IsGlossy(flags);
}

bool BSDFSample::IsSpecular() const {
	return ::IsSpecular(flags);
}