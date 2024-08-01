
#include "BxDF.h"

Vec3 BxDF::rho(Vec3 wo, std::span<const Float> uc, std::span<const Vec2> u2) const {
	if (wo.z == 0)
		return {};
	Vec3 r(0.0f);
	for (size_t i = 0; i < uc.size(); ++i) {
		std::optional<BSDFSample> bs = Sample_f(wo, uc[i], u2[i]);
		if (bs && bs->pdf > 0)
			r += bs->f * AbsCosTheta(bs->wi) / bs->pdf;
	}
	return r / (Float)uc.size();
}
