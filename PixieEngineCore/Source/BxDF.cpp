#include "pch.h"
#include "BxDF.h"

Spectrum BxDF::rho(Vec3 wo, const std::vector<Float>& uc, const std::vector<Vec2>& u2) const {
	glm::fvec3 r(0.0f);
	for (size_t i = 0; i < uc.size(); i++) {
		BSDFSample bs = SampleDirectionAndDistribution(wo, uc[i], u2[i]);
		if (bs.pdf > 0.0f) {
			r += bs.f * AbsCosTheta(bs.wi) / bs.pdf;
		}
	}
	return Spectrum(r / (Float)uc.size());
}

void BxDF::Regularize() {}