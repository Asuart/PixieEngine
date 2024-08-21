#include "pch.h"
#include "BSDFSample.h"

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

BSDFSample::operator bool() const {
	return pdf && f;
}